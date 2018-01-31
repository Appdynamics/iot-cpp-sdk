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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common_test.hpp"
#include "http_mock_interface.hpp"

static appd_iot_http_resp_t global_http_response_mock;
static bool global_http_req_send_cb_triggered;
static bool global_http_req_check_app_status_cb_triggered;
static bool global_http_resp_done_cb_triggered;
static bool global_http_req_bt_header_present;


/**
 * @brief Set Response code for mock http response
 */
void appd_iot_set_response_code(int resp_code)
{
  global_http_response_mock.resp_code = resp_code;
}

/**
 * @brief Set Response headers for mock http response <br>
 * The headers will be freed as part of appd_iot_clear_http_response()
 */
void appd_iot_set_response_headers(int headers_count, appd_iot_data_t* headers)
{
  if ((headers_count == 0) || (headers == NULL))
  {
    fprintf(stdout, "invalid header data\n");
    return;
  }

  global_http_response_mock.headers_count = headers_count;
  global_http_response_mock.headers = (appd_iot_data_t*)calloc(headers_count, sizeof(appd_iot_data_t));
  int header_index = 0;

  for (int i = 0; i < headers_count; i++)
  {
    if ((headers + i) == NULL)
    {
      continue;
    }

    if (headers[i].key == NULL || headers[i].strval == NULL ||
        headers[i].value_type != APPD_IOT_STRING)
    {
      continue;
    }

    size_t key_length = strlen(headers[i].key);
    char* key = (char*)calloc(1, key_length + 1);

    if (key == NULL)
    {
      continue;
    }

    strncpy(key, headers[i].key, key_length);
    key[key_length] = '\0';

    size_t value_length = strlen(headers[i].strval);
    char* value = (char*)calloc(1, value_length + 1);

    if (value == NULL)
    {
      continue;
    }

    strncpy(value, headers[i].strval, value_length);
    value[value_length] = '\0';

    appd_iot_data_set_string(&global_http_response_mock.headers[header_index], key, value);
    header_index++;
  }

  if (header_index != headers_count)
  {
    appd_iot_data_t* temp = (appd_iot_data_t*)realloc(global_http_response_mock.headers,
                            header_index * sizeof(appd_iot_data_t));

    if (temp == NULL)
    {
      global_http_response_mock.headers_count = 0;
      free(global_http_response_mock.headers);
      global_http_response_mock.headers = NULL;
    }
    else
    {
      global_http_response_mock.headers = temp;
      global_http_response_mock.headers_count = header_index;
    }
  }
}

/**
 * @brief Set Response content for mock http response
 */
void appd_iot_set_response_content(const char* src_content)
{
  if (src_content == NULL)
  {
    fprintf(stdout, "http response content is null\n");
    return;
  }

  size_t content_len = strlen(src_content);
  char* dest_content = (char*)calloc(1, content_len + 1);

  strncpy(dest_content, src_content, content_len);
  dest_content[content_len] = '\0';

  global_http_response_mock.content = dest_content;
}

/**
 * @brief Set return code for mock http request
 */
void appd_iot_set_http_req_return_code(appd_iot_error_code_t retcode)
{
  global_http_response_mock.error = retcode;
}

/**
 * @brief Get mock http response
 */
static void appd_iot_get_http_response(appd_iot_http_resp_t** http_resp)
{
  (*http_resp) = &global_http_response_mock;
}

/**
 * @brief Clear mock http response by freeing any memory allocated
 */
static void appd_iot_clear_http_response(appd_iot_http_resp_t* http_resp)
{
  if (http_resp == NULL)
  {
    return;
  }

  if (http_resp->headers != NULL)
  {
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
  }

  free(http_resp->headers);
  free((void*)http_resp->content);
  free((void*)http_resp->user_data);

  http_resp->headers = NULL;
  http_resp->content = NULL;
  http_resp->user_data = NULL;
  http_resp->resp_code = 0;
  http_resp->headers_count = 0;
  http_resp->error = APPD_IOT_SUCCESS;
}


/**
 * @brief Set BT Headers present flag if they are part of beacon payload
 */
static void appd_iot_set_http_req_bt_headers_present(bool status)
{
  global_http_req_bt_header_present = status;
}

/**
 * @brief Indicates if BT headers are present in beacon payload
 */
bool appd_iot_is_http_req_bt_headers_present(void)
{
  return global_http_req_bt_header_present;
}

/**
 * @brief Reset flag that indicates if BT headers are present in beacon payload
 */
void appd_iot_clear_http_req_bt_headers_present_flag(void)
{
  appd_iot_set_http_req_bt_headers_present(false);
}

/**
 * @brief Set http req send callback triggered flag
 */
static void appd_iot_set_http_req_send_cb_triggered(bool status)
{
  global_http_req_send_cb_triggered = status;
}

/**
 * @brief Set http req check app status callback triggered flag
 */
static void appd_iot_set_http_req_check_app_status_cb_triggered(bool status)
{
  global_http_req_check_app_status_cb_triggered = status;
}

/**
 * @brief Set http resp done callback triggered flag
 */
static void appd_iot_set_http_resp_done_cb_triggered(bool status)
{
  global_http_resp_done_cb_triggered = status;
}

/**
 * @brief Check if http req send callback is triggered
 */
bool appd_iot_is_http_req_send_cb_triggered()
{
  return global_http_req_send_cb_triggered;
}

/**
 * @brief Check if http req check app status callback is triggered
 */
bool appd_iot_is_http_req_check_app_status_cb_triggered()
{
  return global_http_req_check_app_status_cb_triggered;
}


/**
 * @brief Check if http resp done callback is triggered
 */
bool appd_iot_is_http_resp_done_cb_triggered()
{
  return global_http_resp_done_cb_triggered;
}

/**
 * @brief Clear flags tracking if callbacks are triggered
 */
void appd_iot_clear_http_cb_triggered_flags()
{
  appd_iot_set_http_req_send_cb_triggered(false);
  appd_iot_set_http_resp_done_cb_triggered(false);
  appd_iot_set_http_req_check_app_status_cb_triggered(false);
}


/**
 * @brief Test Http Request Parameters <br>
 * @return bool flag set to true if http request parameters are valid
 */
static bool appd_iot_validate_http_req(const appd_iot_http_req_t* http_req)
{
  if (http_req == NULL)
  {
    fprintf(stdout, "http_req is null\n");
    return false;
  }

  if (http_req->url == NULL || http_req->type == NULL ||
      http_req->headers == NULL || http_req->data == NULL)
  {
    fprintf(stdout, "null params found in http_req\n");
    return false;
  }

  char buf[128];
  snprintf(buf, sizeof(buf), "%s%s%s%s", TEST_EUM_COLLECTOR_URL,
           TEST_EUM_COLLECTOR_URL_APP_KEY_PREFIX,
           TEST_APP_KEY,
           TEST_EUM_COLLECTOR_URL_APP_KEY_SUFFIX);


  buf[127] = '\0';

  if (strcmp(buf, http_req->url) != 0)
  {
    fprintf(stdout, "Invalid http req url (input:%s) (expected:%s)\n", http_req->url, buf);
    return false;
  }

  if (strcmp(http_req->type, "POST") != 0)
  {
    fprintf(stdout, "Invalid http req type\n");
    return false;
  }

  //check for BT Headers
  if ((strstr(http_req->data, TEST_ADRUM_0) != NULL) &&
      (strstr(http_req->data, TEST_ADRUM_1) != NULL) &&
      (strstr(http_req->data, TEST_ADRUM_2) != NULL) &&
      (strstr(http_req->data, TEST_ADRUM_3) != NULL))
  {
    appd_iot_set_http_req_bt_headers_present(true);
  }
  else
  {
    appd_iot_set_http_req_bt_headers_present(false);
  }

  //verify if sdk version is set
  if ((strstr(http_req->data, TEST_AGENT_VERSION_KEY) == NULL) ||
      (strstr(http_req->data, TEST_SDK_VERSION) == NULL))
  {
    return false;
  }

  return true;
}


/**
 * @brief Test Http Request Parameters for Check App Status GET Request
 * @return bool flag set to true if http request parameters are valid
 */
static bool appd_iot_validate_http_req_check_app_status(const appd_iot_http_req_t* http_req)
{
  if (http_req == NULL)
  {
    fprintf(stdout, "http_req is null\n");
    return false;
  }

  if (http_req->url == NULL || http_req->type == NULL)
  {
    fprintf(stdout, "null params found in http_req\n");
    return false;
  }

  char buf[128];
  snprintf(buf, sizeof(buf), "%s%s%s%s", TEST_EUM_COLLECTOR_URL,
           TEST_EUM_COLLECTOR_URL_APP_KEY_PREFIX,
           TEST_APP_KEY,
           TEST_EUM_COLLECTOR_URL_ENABLED_SUFFIX);


  buf[127] = '\0';

  if (strcmp(buf, http_req->url) != 0)
  {
    fprintf(stdout, "Invalid http req url (input:%s) (expected:%s)\n", http_req->url, buf);
    return false;
  }

  if (strcmp(http_req->type, "GET") != 0)
  {
    fprintf(stdout, "Invalid http req type\n");
    return false;
  }

  return true;

}

/**
 * @brief Http Request Send Callback Function <br>
 * This function mocks an actual http request and returns a http response
 * @return appd_iot_http_resp_t containing response details
 */
appd_iot_http_resp_t* appd_iot_test_http_req_send_cb(const appd_iot_http_req_t* http_req)
{
  appd_iot_http_resp_t* http_resp = NULL;

  appd_iot_set_http_req_send_cb_triggered(true);

  appd_iot_get_http_response(&http_resp);

  bool valid_http_req_check = appd_iot_validate_http_req(http_req);

  if (!valid_http_req_check)
  {
    http_resp->error = APPD_IOT_ERR_INVALID_INPUT;
  }

  return http_resp;
}

/**
 * @brief Http Request Check App Status Callback Function.
 * This function mocks an actual http request and returns a http response
 * @return appd_iot_http_resp_t containing response details
 */
appd_iot_http_resp_t* appd_iot_test_http_req_check_app_status_cb(const appd_iot_http_req_t* http_req)
{
  appd_iot_http_resp_t* http_resp = NULL;

  appd_iot_set_http_req_check_app_status_cb_triggered(true);

  appd_iot_get_http_response(&http_resp);

  bool valid_http_req_check = appd_iot_validate_http_req_check_app_status(http_req);

  if (!valid_http_req_check)
  {
    http_resp->error = APPD_IOT_ERR_INVALID_INPUT;
  }

  return http_resp;
}


/**
 * @brief Http response done Callback Function <br>
 * This function clears the mock http response
 */
void appd_iot_test_http_resp_done_cb(appd_iot_http_resp_t* http_resp)
{
  appd_iot_set_http_resp_done_cb_triggered(true);

  appd_iot_clear_http_response(http_resp);
}
