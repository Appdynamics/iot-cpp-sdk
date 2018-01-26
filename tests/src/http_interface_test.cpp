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

#include <cgreen/cgreen.h>
#include <appd_iot_interface.h>
#include <time.h>
#include <unistd.h>
#include "common_test.hpp"
#include "http_mock_interface.hpp"
#include "log_mock_interface.hpp"
#include "app_mock_interface.hpp"

using namespace cgreen;

Describe(http_interface);
BeforeEach(http_interface) { }
AfterEach(http_interface) { }

/**
 * @brief Unit Test for valid http response
 */
Ensure(http_interface, returns_success_on_valid_appd_iot_http_response)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  //init sdk
  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //add custom event
  appd_iot_custom_event_t custom_event_1;

  appd_iot_init_to_zero(&custom_event_1, sizeof(appd_iot_custom_event_t));

  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //register network interface
  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  http_cb.http_resp_done_cb = &appd_iot_test_http_resp_done_cb;

  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //set http response and send all events
  appd_iot_data_t* resp_headers = NULL;
  int resp_headers_count = 3;

  resp_headers = (appd_iot_data_t*)calloc(resp_headers_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&resp_headers[0], "Pragma", "no-cache");
  appd_iot_data_set_string(&resp_headers[1], "Transfer-Encoding", "chunked");
  appd_iot_data_set_string(&resp_headers[2], "Expires", "0");

  appd_iot_set_response_code(202);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test if callbacks are triggered
  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  appd_iot_clear_http_cb_triggered_flags();

  //test for null response headers, zero header count
  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_set_response_code(202);
  appd_iot_set_response_headers(0, NULL);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  appd_iot_clear_http_cb_triggered_flags();

  //test for no events
  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(false));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(false));

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));

  free(resp_headers);
}


Ensure(http_interface, returns_failure_on_bad_appd_iot_http_response)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;
  sdkcfg.sdk_state_change_cb = NULL;

  devcfg.device_id = "5555";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_custom_event_t custom_event_1;

  appd_iot_init_to_zero(&custom_event_1, sizeof(appd_iot_custom_event_t));

  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test with no http interface registered
  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_NOT_AVAILABLE));

  //register for http interface
  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  http_cb.http_resp_done_cb = &appd_iot_test_http_resp_done_cb;

  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_data_t* resp_headers = NULL;
  int resp_headers_count = 3;
  resp_headers = (appd_iot_data_t*)calloc(resp_headers_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&resp_headers[0], "Pragma", "no-cache");
  appd_iot_data_set_string(&resp_headers[1], "Transfer-Encoding", "chunked");
  appd_iot_data_set_string(&resp_headers[2], "Expires", "0");

  //test for unsuccessful response (400)
  appd_iot_set_response_code(400);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_ERROR));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  //test for unsuccessful response (404)
  appd_iot_set_response_code(404);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_ERROR));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  //test for unsuccessful response (505)
  appd_iot_set_response_code(500);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_ERROR));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  //invalid response code
  appd_iot_set_response_code(3506);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_ERROR));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  //http request return error code
  appd_iot_set_response_code(0);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_set_http_req_return_code(APPD_IOT_ERR_NULL_PTR);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NULL_PTR));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  //test for successful response (202) after above failures
  appd_iot_set_response_code(202);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();
  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  appd_iot_clear_http_cb_triggered_flags();

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));

  free(resp_headers);
}

Ensure(http_interface, check_appd_iot_sdk_disabled_data_limit)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.sdk_state_change_cb = &appd_iot_mock_sdk_state_change_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_custom_event_t custom_event_1;

  appd_iot_init_to_zero(&custom_event_1, sizeof(appd_iot_custom_event_t));

  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);

  //register for http interface
  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  http_cb.http_resp_done_cb = &appd_iot_test_http_resp_done_cb;

  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_data_t* resp_headers = NULL;
  int resp_headers_count = 3;
  resp_headers = (appd_iot_data_t*)calloc(resp_headers_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&resp_headers[0], "Pragma", "no-cache");
  appd_iot_data_set_string(&resp_headers[1], "Transfer-Encoding", "chunked");
  appd_iot_data_set_string(&resp_headers[2], "Expires", "0");

  //test for app disabled response (429)
  appd_iot_set_response_code(429);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_REJECT));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state(), is_equal_to(APPD_IOT_SDK_DISABLED_DATA_LIMIT_EXCEEDED));

  free(resp_headers);
}

Ensure(http_interface, check_appd_iot_sdk_disabled_license_expired)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.sdk_state_change_cb = &appd_iot_mock_sdk_state_change_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_custom_event_t custom_event_1;

  appd_iot_init_to_zero(&custom_event_1, sizeof(appd_iot_custom_event_t));

  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);

  //register for http interface
  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  http_cb.http_resp_done_cb = &appd_iot_test_http_resp_done_cb;

  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_data_t* resp_headers = NULL;
  int resp_headers_count = 3;
  resp_headers = (appd_iot_data_t*)calloc(resp_headers_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&resp_headers[0], "Pragma", "no-cache");
  appd_iot_data_set_string(&resp_headers[1], "Transfer-Encoding", "chunked");
  appd_iot_data_set_string(&resp_headers[2], "Expires", "0");

  //test for app disabled response (402)
  appd_iot_set_response_code(402);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_REJECT));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state(), is_equal_to(APPD_IOT_SDK_DISABLED_LICENSE_EXPIRED));

  free(resp_headers);
}

Ensure(http_interface, check_appd_iot_sdk_disabled_kill_switch_and_enabled)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.sdk_state_change_cb = &appd_iot_mock_sdk_state_change_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  appd_iot_mock_set_sdk_state_change_cb_triggered(false);

  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state(), is_equal_to(APPD_IOT_SDK_ENABLED));

  appd_iot_custom_event_t custom_event_1;

  appd_iot_init_to_zero(&custom_event_1, sizeof(appd_iot_custom_event_t));

  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);

  //register for http interface
  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  http_cb.http_resp_done_cb = &appd_iot_test_http_resp_done_cb;

  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_data_t* resp_headers = NULL;
  int resp_headers_count = 3;
  resp_headers = (appd_iot_data_t*)calloc(resp_headers_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&resp_headers[0], "Pragma", "no-cache");
  appd_iot_data_set_string(&resp_headers[1], "Transfer-Encoding", "chunked");
  appd_iot_data_set_string(&resp_headers[2], "Expires", "0");

  //test for app disabled response (403)
  appd_iot_set_response_code(403);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_REJECT));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state(), is_equal_to(APPD_IOT_SDK_DISABLED_KILL_SWITCH));

  //test for add and send API after SDK is disabled
  appd_iot_clear_http_cb_triggered_flags();
  appd_iot_mock_set_sdk_state_change_cb_triggered(false);

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_SDK_NOT_ENABLED));

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_SDK_NOT_ENABLED));

  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(false));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(false));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(false));

  //register for check app status API http interface
  http_cb.http_req_send_cb = &appd_iot_test_http_req_check_app_status_cb;
  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for check app status api with disabled response
  appd_iot_set_response_code(403);
  appd_iot_clear_http_cb_triggered_flags();
  appd_iot_mock_set_sdk_state_change_cb_triggered(false);

  retcode = appd_iot_check_app_status();
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NETWORK_REJECT));

  assert_that(appd_iot_is_http_req_check_app_status_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(false));

  //test for update app status api with success response
  appd_iot_set_response_code(200);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_check_app_status();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  assert_that(appd_iot_is_http_req_check_app_status_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state_change_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_mock_get_sdk_state(), is_equal_to(APPD_IOT_SDK_ENABLED));

  //register for send beacons http interface
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for successful beacon response
  appd_iot_set_response_code(202);
  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  free(resp_headers);
}

/**
 * @brief Unit Tests for valid http response
 */
TestSuite* http_interface_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, http_interface, returns_success_on_valid_appd_iot_http_response);
  add_test_with_context(suite, http_interface, returns_failure_on_bad_appd_iot_http_response);
  add_test_with_context(suite, http_interface, check_appd_iot_sdk_disabled_data_limit);
  add_test_with_context(suite, http_interface, check_appd_iot_sdk_disabled_license_expired);
  add_test_with_context(suite, http_interface, check_appd_iot_sdk_disabled_kill_switch_and_enabled);

  return suite;
}

