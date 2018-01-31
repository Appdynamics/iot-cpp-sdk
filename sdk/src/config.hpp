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

#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <string>
#include <appd_iot_interface.h>

typedef struct
{
  std::string appkey;             /* Application License Key */
  std::string eum_collector_url;  /* URL where the beacons will be sent to */
  std::string eum_appkey_enabled_url; /* URL to check if application is enabled */
  appd_iot_log_write_cb_t log_write_cb; /* Callback function to write log messages */
  appd_iot_sdk_state_change_cb_t sdk_state_change_cb; /* Callback function to indicate sdk is disabled */
  appd_iot_log_level_t log_level; /* Set Log Level */
  bool initialized;               /* Indicates if config is valid and initialized */
  appd_iot_http_cb_t http_cb;     /* Callback function pointers used to send http req */
} appd_sdk_config_t;

/**
 * @brief Set SDK State
 * @param new_state indicates the new sdk state that need to be set
 */
void appd_iot_set_sdk_state(appd_iot_sdk_state_t new_state);

/**
 * @brief Get Current SDK State
 * @return appd_iot_sdk_state_t with current sdk state
 */
appd_iot_sdk_state_t appd_iot_get_sdk_state(void);

/**
 * @brief Set SDK state to disabled state based on the HTTP Response Code
 * @param http_resp_code indicates the response code from the Collector
 */
void appd_iot_disable_sdk(int http_resp_code);

/**
  * @brief Get configured Log Level as part of SDK Initialization
  * @return appd_iot_log_level_t contains log level enum
  */
appd_iot_log_level_t appd_iot_get_log_level(void);


/**
  * @brief Get Configured EUM Collector URL
  * @return URL in string format
  */
const char* appd_iot_get_eum_collector_url(void);


/**
 * @brief Get Log Write Callback Function Pointer.
 * @return appd_iot_log_write_cb_t contains log_write_cb fun ptr
 */
appd_iot_log_write_cb_t appd_iot_get_log_write_cb(void);


/**
 * @brief Get http request send callback function pointer
 * @return http request send callback function pointer
 */
appd_iot_http_req_send_cb_t appd_iot_get_http_req_send_cb(void);


/**
 * @brief Get http response done callback function pointer
 * @return http response done callback function pointer
 */
appd_iot_http_resp_done_cb_t appd_iot_get_http_resp_done_cb(void);


#endif // _CONFIG_HPP_
