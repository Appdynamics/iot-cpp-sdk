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

#ifndef http_mock_interface_hpp
#define http_mock_interface_hpp

#include <appd_iot_interface.h>

/**
 * @brief Set Response code for mock http response
 */
void appd_iot_set_response_code(int resp_code);

/**
 * @brief Set Response headers for mock http response
 */
void appd_iot_set_response_headers(int headers_count, appd_iot_data_t* headers);

/**
 * @brief Set Response content for mock http response
 */
void appd_iot_set_response_content(const char* content);

/**
 * @brief Set mock http request return code
 */
void appd_iot_set_http_req_return_code(appd_iot_error_code_t retcode);

/**
 * @brief Indicates if BT headers are present in beacon payload
 */
bool appd_iot_is_http_req_bt_headers_present(void);

/**
 * @brief Reset flag that indicates if BT headers are present in beacon payload
 */
void appd_iot_clear_http_req_bt_headers_present_flag(void);

/**
 * @brief Check if http req send callback is triggered
 */
bool appd_iot_is_http_req_send_cb_triggered();

/**
 * @brief Check if http req check app status callback is triggered
 */
bool appd_iot_is_http_req_check_app_status_cb_triggered();

/**
 * @brief Check if http resp done callback is triggered
 */
bool appd_iot_is_http_resp_done_cb_triggered();

/**
 * @brief Clear flags tracking if callbacks are triggered
 */
void appd_iot_clear_http_cb_triggered_flags();

/**
 * @brief Http Request Send Callback Function <br>
 * This function mocks an actual http request and returns a http response
 * @return appd_iot_http_resp_t containing response details
 */
appd_iot_http_resp_t* appd_iot_test_http_req_send_cb(const appd_iot_http_req_t* http_req);


/**
 * @brief Http Request Check App Status Callback Function <br>
 * This function mocks an actual http request and returns a http response
 * @return appd_iot_http_resp_t containing response details
 */
appd_iot_http_resp_t* appd_iot_test_http_req_check_app_status_cb(const appd_iot_http_req_t* http_req);

/**
 * @brief Http response done Callback Function <br>
 * This function clears the mock http response
 */
void appd_iot_test_http_resp_done_cb(appd_iot_http_resp_t* http_resp);


#endif /* http_mock_interface_hpp */
