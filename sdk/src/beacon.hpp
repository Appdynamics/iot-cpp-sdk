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

#ifndef _BEACON_HPP
#define _BEACON_HPP

#include <string>
#include <map>
#include <list>
#include <appd_iot_interface.h>

#define APPD_IOT_MAX_CUSTOM_EVENTS 200
#define APPD_IOT_MAX_NETWORK_EVENTS 200
#define APPD_IOT_MAX_ERROR_EVENTS 200

typedef struct
{
  std::map<std::string, std::string> stringmap;
  std::map<std::string, int64_t>integermap;
  std::map<std::string, double>doublemap;
  std::map<std::string, bool>boolmap;
  std::map<std::string, int64_t>datetimemap;
} data_t;

typedef struct
{
  std::string type; /* Type of the event  */
  std::string summary; /* Summary of the event  */
  int64_t timestamp_ms;  /*  Timestamp UTC format in milliseconds */
  int duration_ms; /* Duration of the event in milliseconds */
  data_t data;
} custom_event_t;

typedef struct
{
  std::string url;
  std::string error;
  int req_content_length;
  int resp_content_length;
  int resp_code;
  int64_t timestamp_ms;
  int duration_ms;
  data_t resp_headers;
  data_t data;
} network_request_event_t;

typedef struct
{
  std::string symbol_name;
  std::string package_name;
  std::string file_name;
  int lineno;
  uint64_t absolute_addr;
  int image_offset;
  int symbol_offset;
} stack_frame_t;

typedef struct
{
  std::string thread;
  std::string runtime;
  std::list<stack_frame_t> stack_frame_list;
} stack_trace_t;

typedef struct
{
  std::string name;
  std::string message;
  std::string severity;
  int64_t timestamp_ms;
  int duration_ms;
  int error_stack_trace_index;
  std::list<stack_trace_t> stack_trace_list;
  data_t data;
} error_event_t;

typedef struct
{
  std::string device_name;
  std::string device_type;
  std::string device_id;
  std::string hw_version;
  std::string fw_version;
  std::string sw_version;
  std::string os_version;
} device_cfg_t;

typedef struct
{
  device_cfg_t devcfg;
  std::list<custom_event_t> custom_event_list;
  std::list<network_request_event_t> network_request_event_list;
  std::list<error_event_t> error_event_list;
} beacon_t;

/**
 * @brief Initializes Device Configuration <br>
 * It is madatory to set Device ID and Device Type.
 * @param devcfg contains device configuration
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_init_device_config(appd_iot_device_config_t devcfg);


/**
  * @brief Adds Custom Event to Beacon
  * @param event contains custom event data to be sent to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_custom_event_to_beacon(custom_event_t event);


/**
  * @brief Adds Network Request Event to Beacon
  * @param event contains network request event data to be sent to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_network_request_event_to_beacon(network_request_event_t event);


/**
  * @brief Adds Error Event to Beacon
  * @param event contains error event data to be sent to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_error_event_to_beacon(error_event_t event);


/**
  * @brief Sends Beacons in memory to collector.
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_send_all_beacons(void);


/**
  * @brief Clears Beacons in memory
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_clear_all_beacons(void);

#endif // _BEACON_HPP
