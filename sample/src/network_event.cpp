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

#include "network_event.hpp"
#include "http_curl_interface.hpp"

/**
 * @brief Fills a network event structure and <br>
 * calls IoT CPP SDK API to add and send the event
 */
void send_network_event(void)
{
  appd_iot_network_request_event_t network_event;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&network_event, sizeof(appd_iot_network_request_event_t));

  network_event.url = "https://apdy.api.com/weather";
  network_event.resp_code = 202;
  network_event.duration_ms = 10;
  network_event.req_content_length = 300;
  network_event.resp_content_length = 100;
  network_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  network_event.error = NULL;
  network_event.data_count = 5;
  network_event.data = (appd_iot_data_t*)calloc(network_event.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&network_event.data[0], "city", "San Francisco");
  appd_iot_data_set_string(&network_event.data[1], "country", "USA");
  appd_iot_data_set_integer(&network_event.data[2], "zip", 94107);
  appd_iot_data_set_double(&network_event.data[3], "lat", 37.30);
  appd_iot_data_set_double(&network_event.data[4], "long", -122.39);

  retcode = appd_iot_add_network_request_event(network_event);
  fprintf(stdout, "Add Network Event Status :%s\n", appd_iot_error_code_to_str(retcode));

  free(network_event.data);

  if (retcode == APPD_IOT_SUCCESS)
  {
    retcode = appd_iot_send_all_events();
    fprintf(stdout, "Send Network Event Status :%s\n\n", appd_iot_error_code_to_str(retcode));
  }
}


/* @brief Trigger HTTP Request to server URL and capture network event <br>
 * Send the captured network event to collector.
 * @param url to which a HTTP Request is triggered
 * @param reqtype specifies the type of HTTP Request (GET or POST)
 * @param reqdata specifies any data that needs to be sent as part of HTTP Request
 */
void capture_and_send_network_event(const char* url, const char* type, const char* data)
{
  /* Init all the data structures - REQ and RESP */
  appd_iot_http_req_t http_req;
  appd_iot_http_resp_t* http_resp = NULL;
  struct timeval starttime, endtime, duration;

  appd_iot_init_to_zero(&http_req, sizeof(http_req));

  http_req.type = type;
  http_req.url = url;
  http_req.data = data;
  http_req.headers_count = 2 + APPD_IOT_NUM_SERVER_CORRELATION_HEADERS;
  http_req.headers = (appd_iot_data_t*)calloc(http_req.headers_count, sizeof(appd_iot_data_t));

  if (http_req.headers == NULL)
  {
    fprintf(stderr, "Memory allocation failed \n");
    return;
  }

  appd_iot_data_set_string(&http_req.headers[0], "Accept", "application/json");
  appd_iot_data_set_string(&http_req.headers[1], "Content-Type", "application/json");

  const appd_iot_data_t* correlation_headers = appd_iot_get_server_correlation_headers();

  int req_idx = 2;

  for (size_t i = 0; i < APPD_IOT_NUM_SERVER_CORRELATION_HEADERS; i++)
  {
    appd_iot_data_set_string(&http_req.headers[req_idx],
                             correlation_headers[i].key, correlation_headers[i].strval);
    req_idx++;
  }

  gettimeofday(&starttime, NULL);

  http_resp = http_curl_req_send_cb(&http_req);

  gettimeofday(&endtime, NULL);

  free(http_req.headers);

  //populate the event and send it
  duration.tv_sec = (endtime.tv_sec - starttime.tv_sec);
  duration.tv_usec = (endtime.tv_usec - starttime.tv_usec);

  int duration_ms = (int)(duration.tv_sec * 1000) + (int)(duration.tv_usec / 1000);

  appd_iot_network_request_event_t network_event;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&network_event, sizeof(appd_iot_network_request_event_t));

  network_event.url = url;
  network_event.resp_code = http_resp->resp_code;
  network_event.duration_ms = duration_ms;
  network_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  network_event.resp_headers_count = http_resp->headers_count;
  network_event.resp_headers = http_resp->headers;
  network_event.resp_content_length = http_resp->content_len;

  if (data != NULL)
  {
    network_event.req_content_length = strlen(data);
  }

  if (http_resp->error != APPD_IOT_SUCCESS)
  {
    network_event.error = appd_iot_error_code_to_str(http_resp->error);
  }

  retcode = appd_iot_add_network_request_event(network_event);
  fprintf(stdout, "Add Network Event Status :%s\n", appd_iot_error_code_to_str(retcode));

  http_curl_resp_done_cb(http_resp);

  if (retcode == APPD_IOT_SUCCESS)
  {
    retcode = appd_iot_send_all_events();
    fprintf(stdout, "Send Network Event Status :%s\n\n", appd_iot_error_code_to_str(retcode));
  }

}

