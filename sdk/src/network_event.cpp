/*
 * Copyright (c) 2018 AppDynamics, Inc., and its affiliates
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

#include "custom_event.hpp"
#include "log.hpp"
#include "config.hpp"

/**
 * @brief checks if http response code is valid
 * @return true if http response code is valid
 */
static bool appd_iot_is_valid_http_resp_code(int resp_code)
{
  return (resp_code >= 100 && resp_code < 600);
}

/**
 * @brief Copies User Defined Response Header Data to SDK Defined Structure
 * @param dest_respheader to which response header data is copied to
 * @param src_respheader contains response header data to be copied from
 * @param src_respheadercount contains number of key-value pairs in response header data
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_copy_response_headers_data
(data_t* dest_respheader, appd_iot_data_t* src_respheader, int src_respheadercount)
{
  if (dest_respheader == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Response Header Destination Data Pointer in NULL");
    return APPD_IOT_ERR_INTERNAL;
  }

  for (int i = 0; i < src_respheadercount; i++)
  {
    if (src_respheader[i].key == NULL)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Response Header <Key> at Index:%d is NULL", i);
      return APPD_IOT_ERR_NULL_PTR;
    }

    std::string key = src_respheader[i].key;

    //skip if it's not a adrum header
    if ((key.compare(0, 5, "ADRUM")) &&
        (key.compare(0, 5, "adrum")))
    {
      continue;
    }

    //Expect only string data in response headers
    if (src_respheader[i].value_type != APPD_IOT_STRING)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "ADRUM Header Value is expected \
                   to be of string data type but found data type:%d", src_respheader[i].value_type);
      continue;
    }

    //Remove any leading space in the header value
    int idx = 0;

    while (isspace(src_respheader[i].strval[idx]))
    {
      idx++;
    }

    dest_respheader->stringmap[key] = (src_respheader[i].strval + idx);

    appd_iot_log(APPD_IOT_LOG_INFO, "Added Response Header Key :%s with value :%s",
                 src_respheader[i].key,
                 src_respheader[i].strval);
  }

  return APPD_IOT_SUCCESS;
}


/**
  * @brief converts network event data to beacon format and adds to beacon
  * @param network_request_event contains network event data
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_network_request_event
(appd_iot_network_request_event_t network_request_event)
{
  appd_iot_error_code_t retcode;
  appd_iot_sdk_state_t sdk_state;
  network_request_event_t event;

  if ((sdk_state = appd_iot_get_sdk_state()) != APPD_IOT_SDK_ENABLED)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Add Network Event Failed. SDK Not in Enabled State:%s",
                 appd_iot_sdk_state_to_str(sdk_state));

    return APPD_IOT_ERR_SDK_NOT_ENABLED;
  }

  if (network_request_event.url == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "URL field cannot be NULL");
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  if (network_request_event.error == NULL &&
      !appd_iot_is_valid_http_resp_code(network_request_event.resp_code))
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Network Error or valid Response Code needs to be populated");
  }

  if (strncmp(network_request_event.url,
              appd_iot_get_eum_collector_url(),
              strlen(network_request_event.url)) == 0)
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Skip Adding Network Event with Event URL same as APPD EUM Collector URL:%s",
                 network_request_event.url);
    return APPD_IOT_ERR_NOT_SUPPORTED;
  }

  retcode = APPD_IOT_SUCCESS;
  event.url = network_request_event.url;

  if (network_request_event.error != NULL)
  {
    event.error = network_request_event.error;
  }

  if (appd_iot_is_valid_http_resp_code(network_request_event.resp_code))
  {
    event.resp_code = network_request_event.resp_code;
  }
  else
  {
    event.resp_code = 0;
  }

  event.req_content_length = network_request_event.req_content_length;
  event.resp_content_length = network_request_event.resp_content_length;
  event.timestamp_ms = network_request_event.timestamp_ms;
  event.duration_ms = network_request_event.duration_ms;

  if (network_request_event.resp_headers_count > 0)
  {
    retcode = appd_iot_copy_response_headers_data(&event.resp_headers,
              network_request_event.resp_headers,
              network_request_event.resp_headers_count);

    if (retcode != APPD_IOT_SUCCESS)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to parse network event response headers, error:%s",
                   appd_iot_error_code_to_str(retcode));

      appd_iot_clear_event_data(&event.resp_headers);
    }
  }

  if (network_request_event.data_count > 0)
  {
    retcode = appd_iot_copy_event_data(&event.data, network_request_event.data,
                                       network_request_event.data_count);

    if (retcode != APPD_IOT_SUCCESS)
    {
      appd_iot_log(APPD_IOT_LOG_WARN, "Failed to parse Network event data, error:%s",
                   appd_iot_error_code_to_str(retcode));

      appd_iot_clear_event_data(&event.data);
    }
  }

  appd_iot_log(APPD_IOT_LOG_INFO, "Adding Network Event with URL:%s", event.url.c_str());

  retcode = appd_iot_add_network_request_event_to_beacon(event);

  return retcode;
}
