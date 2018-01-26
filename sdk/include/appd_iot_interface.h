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

/**
 * @file appd_iot_interface.h
 * @brief This file contains public API's to access SDK functionality
 */

#ifndef _APPD_IOT_INTERFACE_H_
#define _APPD_IOT_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! Attribute added to all the API's that need to be
 *  made public outside of appdynamics iot shared dynamic library.
 */
#if __GNUC__ >= 4
#define __APPD_IOT_API __attribute__((visibility("default")))
#else
#define __APPD_IOT_API
#endif

#include "appd_iot_def.h"


/**
 * @brief This method Initializes the SDK. <br>
 * This method must be called before all other SDK Apis are called. <br>
 * It is recommened to call this method early in your application's start up sequence.
 * @param sdkcfg contains sdk configuration such as collector url, appkey etc
 * @param devcfg contains device configuration such as device type/name and hw/fw/sw versions
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_init_sdk(appd_iot_sdk_config_t sdkcfg,
                                        appd_iot_device_config_t devcfg) __APPD_IOT_API;


/**
 * @brief This method registers network interface <br>
 * This method must be called before calling appd_iot_send_all_events().
 * @param http_cb contains function pointers for http req send and http resp done callbacks <br>.
 * http request send callback will be called when appd_iot_send_all_events is triggered. <br>
 * http response done callback will be called after send http req callback returns and resp is processed.
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail. <br>
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_register_network_interface(appd_iot_http_cb_t http_cb) __APPD_IOT_API;


/**
 * @brief This method adds custom event data <br>
 * Each call to add event will create a new event.
 * @param custom_event contains details of the event
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_add_custom_event(appd_iot_custom_event_t custom_event) __APPD_IOT_API;


/**
 * @brief This method adds  event data <br>
 * Each call to add event will create a new event.
 * @param network_request_event contains details of the network request
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_add_network_request_event
(appd_iot_network_request_event_t network_request_event) __APPD_IOT_API;


/**
 * @brief This method adds  event data <br>
 * Each call to add event will create a new event.
 * @param error_event contains details of the error event
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_add_error_event(appd_iot_error_event_t error_event) __APPD_IOT_API;


/**
 * @brief This method sends all event data. <br>
 * If events are sent successfuly to collector then they will be flushed out of memory. <br>
 * If there is a network reject with response codes 402, 403 or 429 then events are flushed out of memory and
 * SDK state set to DISABLED. If there is any other network error, events remain in memory for retry.
 * Repeated calls to this API in SDK ENABLED State will retry sending the data in memory to the collector. <br>
 * Use the API appd_iot_clear_all_events() to clear out events in memory if retries are unsuccessful.
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_send_all_events(void) __APPD_IOT_API;


/**
 * @brief This method removes all event data stored in memory <br>
 * This call is not needed if appd_iot_send_all_events return SUCCESS.
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_clear_all_events(void) __APPD_IOT_API;


/**
 * @brief Use this API to check with AppDynamics Collector on the status of IoT Application on
 * AppDynamics Controller, whether instrumentation is enabled or not. If the Collector returns Success, SDK
 * gets ENABLED in case it has been DISABLED previously by Collector due to license expiry, kill switch or
 * data limit exceeded. <br>
 * It is required that SDK initialization is already done using the API appd_iot_init_sdk() before
 * calling this function.
 * @return appd_iot_error_code_t will indicate success if app is enabled
 */
appd_iot_error_code_t appd_iot_check_app_status(void) __APPD_IOT_API;

#ifdef __cplusplus
} /* extern "C" */
#endif  /* defined(__cplusplus) */

#endif /* _APPD_IOT_INTERFACE_H_ */
