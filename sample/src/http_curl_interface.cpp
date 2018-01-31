/*
 * Copyright (c) 2018 AppDynamics LLC and its affiliates
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include "http_curl_interface.hpp"


/* holder for curl response content */
typedef struct
{
  char* data;
  size_t len;
} content_t;

typedef struct
{
  CURL* ch;
  CURLcode respcode;
  int num_resp_headers;
  struct curl_slist* req_headers;
  struct curl_slist* resp_headers;
  content_t content;
} curl_handle_t;


/**
 * @brief frees CURL data structures used for http req and response.
 * @param curl_handle containing http req and resp details
 */
static void http_curl_deinit(curl_handle_t* curl_handle)
{
  if (curl_handle != NULL)
  {
    if (curl_handle->ch != NULL)
    {
      curl_easy_cleanup(curl_handle->ch);
    }

    if (curl_handle->req_headers != NULL)
    {
      curl_slist_free_all(curl_handle->req_headers);
    }

    if (curl_handle->content.data != NULL)
    {
      free(curl_handle->content.data);
    }

    if (curl_handle->resp_headers != NULL)
    {
      curl_slist_free_all(curl_handle->resp_headers);
    }

    free(curl_handle);
  }
}


/**
 * @brief This method initializes data structures for CURL http operation
 * @return curl_handle_t contains context of http curl request
 */
static curl_handle_t* http_curl_init(void)
{
  curl_handle_t* curl_handle = (curl_handle_t*)calloc(1, sizeof(curl_handle_t));

  if (curl_handle == NULL)
  {
    /* log error */
    fprintf(stderr, "Failed to create curl handle\n");
    /* return error */
    return NULL;

  }

  curl_handle->req_headers = NULL;
  curl_handle->resp_headers = NULL;
  curl_handle->content.data = NULL;

  /* init curl handle */
  if ((curl_handle->ch = curl_easy_init()) == NULL)
  {
    /* log error */
    fprintf(stderr, "Failed to init curl handle\n");
    http_curl_deinit(curl_handle);
    curl_handle = NULL;
    return NULL;
  }

  return curl_handle;
}

/**
 * @brief This is a callback method triggered upon receiving http response to read resp content
 * @param src_content contains response content
 * @param size contains size of one memory block
 * @param nmemb contains number of memory blocks
 * @param dest_content destination to which response content will be written to
 * @return size_t returns the size of the response content
 */
static size_t http_curl_resp_content_cb(void* src_content, size_t size, size_t nmemb, content_t* dest_content)
{
  size_t new_len = dest_content->len + size * nmemb;
  char* tmp = (char*)realloc(dest_content->data, new_len + 1);

  if (tmp == NULL)
  {
    fprintf(stderr, "realloc() failed\n");

    dest_content->len = 0;
    free(dest_content->data);
    dest_content->data = NULL;

    return 0;
  }

  dest_content->data = tmp;
  memcpy(dest_content->data + dest_content->len, src_content, size * nmemb);
  dest_content->data[new_len] = '\0';
  dest_content->len = new_len;

  return size * nmemb;
}


/**
 * @brief This is a callback method triggered upon receiving http response <br>
 * to read response headers. It is invoked for each response header.
 * @param src_header contains response header which is to be copied
 * @param size contains size of one memory block
 * @param nmemb contains number of memory blocks
 * @param dest_headers destination to which response headers will be copied to
 * @return size_t returns the size of the response headers
 */
static size_t http_curl_resp_headers_cb
(void* src_header, size_t size, size_t nmemb, void* dest_headers)
{
  size_t realsize = size * nmemb; /* calculate buffer size */

  //src_header is not guaranteed to be null terminated. so copy src_headers into local buf
  char buf[realsize + 1];
  snprintf(buf, sizeof(buf), "%s", (char*)src_header);
  buf[realsize] = '\0';

  struct curl_slist** h = (struct curl_slist**)dest_headers;  /* cast pointer to slist struct */

  *h = curl_slist_append(*h, buf);

  if (*h == NULL)
  {
    return 0;
  }

  /* return size */
  return realsize;
}


/**
 * @brief reads and converts response headers into key value pairs
 * @param src_headers response headers in string format
 * @param dest_headers structure into which response headers are populated as a map of key-value pairs
 * @return number of headers parsed
 */
static int http_curl_parse_resp_headers
(struct curl_slist* src_headers, appd_iot_data_t** dest_headers)
{
  const char eol[] = "\r\n";
  const char headerdelimiter = ':';
  char* header = NULL;
  int header_index = 0;
  int num_resp_headers = 0;

  struct curl_slist* iter = src_headers;

  /* Count number of headers which are in Key:Value format */
  while (iter != NULL)
  {
    char* data = iter->data;

    if (strchr(data, headerdelimiter) != NULL)
    {
      num_resp_headers++;
    }

    iter = iter->next;
  }

  if (num_resp_headers == 0)
  {
    fprintf(stderr, "No response headers present in key:value format\n");
    return num_resp_headers;
  }

  fprintf(stdout, "Http Response Headers: %d\n", num_resp_headers);


  appd_iot_data_t* dest_resp_headers = (appd_iot_data_t*)calloc(num_resp_headers, sizeof(appd_iot_data_t));

  if (dest_resp_headers == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for resp headers\n");
    return 0;
  }

  /* Parse each response header in key:value format and split them into key and value */
  iter = src_headers;

  while (iter != NULL)
  {
    if (iter->data == NULL)
    {
      iter = iter->next;
      continue;
    }

    //copy header data into local buffer
    size_t data_len = strlen(iter->data);
    char buf[data_len + 1];
    char* buf_ptr = buf;

    snprintf(buf, sizeof(buf), "%s", iter->data);
    buf[data_len] = '\0';

    /* split header using EOL delimiter */
    header = strsep (&buf_ptr, eol);

    if (header == NULL)
    {
      iter = iter->next;
      continue;
    }

    if (header[0] == '\0')
    {
      iter = iter->next;
      continue;
    }

    size_t header_len = strlen(header);

    /* split header using : delimiter to parse key:value pair */
    char* pch = strchr(header, headerdelimiter);

    if (pch != NULL)
    {
      /* Read Key */
      size_t key_length = (int)(pch - header);
      char* key = (char*)calloc(1, key_length + 1);

      if (key == NULL)
      {
        iter = iter->next;
        continue;
      }

      strncpy(key, header, key_length);
      key[key_length] = '\0';

      /* Read Value */
      size_t value_length;

      //-1 to header_len to skip last char if it's a new line.
      //+1 to key_length to skip delimiter ':'
      if (header[header_len - 1] == '\r' || header[header_len - 1] == '\n')
      {
        value_length = (header_len - 1) - (key_length + 1);
      }
      else
      {
        value_length = header_len - (key_length + 1);
      }

      char* value = (char*)calloc(1, value_length + 1);

      if (value == NULL)
      {
        iter = iter->next;
        free(key);
        continue;
      }

      strncpy(value, pch + 1, value_length);
      value[value_length] = '\0';

      /* Add Key, Value Pair to Response Header */
      appd_iot_data_set_string(&dest_resp_headers[header_index], key, value);

      fprintf(stdout, "Http Response Header:%d (%s:%s)\n", header_index, dest_resp_headers[header_index].key,
              dest_resp_headers[header_index].strval);

      header_index++;
    }

    iter = iter->next;
  }

  //realloc if there is a mismatch in number of headers present vs parsed successfully
  if (header_index != num_resp_headers)
  {
    appd_iot_data_t* temp = (appd_iot_data_t*)realloc(dest_resp_headers,
                            header_index * sizeof(appd_iot_data_t));

    if (temp == NULL)
    {
      free(dest_resp_headers);
      dest_resp_headers = NULL;
      num_resp_headers = 0;
    }
    else
    {
      dest_resp_headers = temp;
      num_resp_headers = header_index;
    }
  }

  (*dest_headers) = dest_resp_headers;

  return num_resp_headers;
}


/**
 * @brief Read curl http resp and fill http_resp structure
 * @param curl_handle contains parameters required to perform curl operation
 * @param http_resp to which curl response details (resp code, headers and content) are copied to
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t http_curl_fill_response
(curl_handle_t* curl_handle, appd_iot_http_resp_t* http_resp)
{
  if (curl_handle == NULL || http_resp == NULL)
  {
    return APPD_IOT_ERR_NULL_PTR;
  }

  struct curl_slist* resp_headers = curl_handle->resp_headers;

  curl_easy_getinfo (curl_handle->ch, CURLINFO_RESPONSE_CODE, &(http_resp->resp_code));

  fprintf(stdout, "Http Response Code:%d\n",  http_resp->resp_code);

  /* check resp headers for null */
  if (resp_headers != NULL)
  {
    /* parse resp headers */
    http_resp->headers_count = http_curl_parse_resp_headers(resp_headers, &(http_resp->headers));
  }
  else
  {
    /* error */
    fprintf(stdout, "Failed to populate Response Header\n");
    return APPD_IOT_ERR_NULL_PTR;
  }

  if (curl_handle->content.len != 0)
  {
    http_resp->content = (char*)calloc(1, curl_handle->content.len + 1);

    if (http_resp->content != NULL)
    {
      memcpy((void*)http_resp->content, curl_handle->content.data, curl_handle->content.len);
      http_resp->content_len = curl_handle->content.len;
    }
    else
    {
      fprintf(stderr, "failed to allocate memory to read content\n");
      http_resp->content = NULL;
      http_resp->content_len = 0;
    }
  }

  return APPD_IOT_SUCCESS;
}

/**
 * @brief Set Options for HTTP Request to be sent by CURL
 * @param curl_handle curl request context
 * @param http_req contains http req options (url, req type, headers etc)
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t http_curl_set_options
(curl_handle_t* curl_handle, const appd_iot_http_req_t* http_req)
{
  /* set request headers */
  for (int i = 0; i < http_req->headers_count; i++)
  {
    if ((http_req->headers + i) == NULL)
    {
      continue;
    }

    if (http_req->headers[i].key == NULL || http_req->headers[i].strval == NULL ||
        http_req->headers[i].value_type != APPD_IOT_STRING)
    {
      fprintf(stderr, "Http Request Headers Not Valid\n");
      continue;
    }

    size_t header_len = strlen(http_req->headers[i].key) + strlen(http_req->headers[i].strval);
    //+1 for ':' and +1 for terminating char
    char header[header_len + 2];
    snprintf(header, sizeof(header), "%s:%s", http_req->headers[i].key, http_req->headers[i].strval);
    header[header_len + 1] = '\0';

    fprintf(stdout, "Http Request Header:%d %s\n", i, header);
    curl_handle->req_headers = curl_slist_append(curl_handle->req_headers, header);
  }

  CURL* ch = curl_handle->ch;

  /* set url to fetch */
  curl_easy_setopt(ch, CURLOPT_URL, http_req->url);

  /* set calback function */
  curl_easy_setopt(ch, CURLOPT_HEADERFUNCTION, http_curl_resp_headers_cb);

  /* pass fetch struct pointer */
  curl_easy_setopt(ch, CURLOPT_HEADERDATA, (void*) & (curl_handle->resp_headers));

  /* set default user agent */
  curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* set timeout */
  curl_easy_setopt(ch, CURLOPT_TIMEOUT, 30L);

  /* enable location redirects */
  curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

  /* set maximum allowed redirects */
  curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

  /* set request headers */
  curl_easy_setopt(ch, CURLOPT_HTTPHEADER, curl_handle->req_headers);

  /* set request type */
  curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, http_req->type);

  /* set post data */
  if (http_req->data != NULL)
  {
    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, http_req->data);
  }

  curl_handle->content.len = 0;

  curl_handle->content.data = NULL;

  /* set callback function */
  curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, http_curl_resp_content_cb);

  /* pass fetch struct pointer */
  curl_easy_setopt(ch, CURLOPT_WRITEDATA, &(curl_handle->content));

  return APPD_IOT_SUCCESS;

}


/**
 * @brief Http Req Send Callback function which uses curl API to send http req <br>
 * @param http_req contains http req details
 * @return http_resp to which response details(resp code, headers and content) are copied to
 */
appd_iot_http_resp_t* http_curl_req_send_cb(const appd_iot_http_req_t* http_req)
{
  /* Initialize curl and set req parameters */
  curl_handle_t* curl_handle = http_curl_init();

  if (curl_handle == NULL)
  {
    return NULL;
  }

  appd_iot_error_code_t error = APPD_IOT_SUCCESS;
  appd_iot_http_resp_t* http_resp = (appd_iot_http_resp_t*)calloc(1, sizeof(appd_iot_http_resp_t));

  if (http_resp == NULL)
  {
    http_curl_deinit(curl_handle);
    return NULL;
  }

  http_resp->user_data = (void*)(curl_handle);

  error = http_curl_set_options(curl_handle, http_req);

  if (error != APPD_IOT_SUCCESS)
  {
    http_resp->error = error;
    return http_resp;
  }

  /* fetch url */
  CURLcode rcode = curl_easy_perform(curl_handle->ch);

  /* read response */
  if (rcode == CURLE_OK)
  {
    error = http_curl_fill_response(curl_handle, http_resp);
  }
  else
  {
    char* url = NULL;

    curl_easy_getinfo(curl_handle->ch, CURLINFO_EFFECTIVE_URL, &url);

    /* log error */
    fprintf(stderr, "Failed to fetch url (%s) - curl said: %s\n", url, curl_easy_strerror(rcode));

    error = APPD_IOT_ERR_NETWORK_UNREACHABLE;
  }

  http_resp->error = error;
  return http_resp;
}


/**
 * @brief http response done callback. <br>
 * This callback is used to clean up any memory allocated while executing http req. <br>
 * Also reset any state variables.
 * @param http_resp contains http resp details
 */
void http_curl_resp_done_cb(appd_iot_http_resp_t* http_resp)
{
  if (http_resp == NULL)
  {
    fprintf(stderr, "Resp Struct is NULL, nothing to free\n");
    return;
  }

  if (http_resp->user_data != NULL)
  {
    http_curl_deinit(( curl_handle_t*)http_resp->user_data);
  }

  for (int i = 0; i < http_resp->headers_count; i++)
  {
    if ((http_resp->headers + i) != NULL)
    {
      if (http_resp->headers[i].key != NULL)
      {
        free((void*)http_resp->headers[i].key);
      }

      if (http_resp->headers[i].value_type == APPD_IOT_STRING && http_resp->headers[i].strval != NULL)
      {
        free((void*)http_resp->headers[i].strval);
      }
    }
  }

  if (http_resp->headers != NULL)
  {
    free(http_resp->headers);
  }

  if (http_resp->content != NULL)
  {
    free((void*)http_resp->content);
  }

  free(http_resp);

  return;
}
