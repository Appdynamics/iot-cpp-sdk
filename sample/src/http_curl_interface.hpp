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

#ifndef _HTTP_CURL_INTERFACE_HPP_
#define _HTTP_CURL_INTERFACE_HPP_

#include <appd_iot_interface.h>


/**
 * @brief Http Req Send Callback function which uses curl API to send http req <br>
 * @param http_req contains http req details
 * @return http_resp to which response details(resp code, headers and content) are copied to
 */
appd_iot_http_resp_t* http_curl_req_send_cb(const appd_iot_http_req_t* http_req);

/**
 * @brief Http Response Done Callback. <br>
 * This callback is used to clean up any memory allocated while executing http req. <br>
 * Also reset any state variables.
 * @param http_resp contains http resp details
 */
void http_curl_resp_done_cb(appd_iot_http_resp_t* http_resp);

#endif //_HTTP_CURL_INTERFACE_HPP_
