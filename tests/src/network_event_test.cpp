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

using namespace cgreen;

Describe(network_event);
BeforeEach(network_event) { }
AfterEach(network_event) { }


/**
 * @brief Unit Test for valid complete network event
 */
Ensure(network_event, returns_success_on_full_appd_iot_add_and_send_network_event)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_network_request_event_t network_event;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "Point of Sale";
  devcfg.device_name = "POS#12";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_init_to_zero(&network_event, sizeof(appd_iot_network_request_event_t));

  network_event.url = "http://apdy.com/processPayment";
  network_event.resp_code = 202;
  network_event.duration_ms = 10;
  network_event.req_content_length = 300;
  network_event.resp_content_length = 100;
  network_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  network_event.error = NULL;
  network_event.data_count = 7;
  network_event.data = (appd_iot_data_t*)calloc(network_event.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&network_event.data[0], "UserID", "12345");
  appd_iot_data_set_string(&network_event.data[1], "UserEmail", "firstname.lastname@gmail.com");
  appd_iot_data_set_integer(&network_event.data[2], "Items Purchases", 12);
  appd_iot_data_set_double(&network_event.data[3], "Total Sale", 77.5);
  appd_iot_data_set_string(&network_event.data[4], "Credit Card Used", "Visa");
  appd_iot_data_set_boolean(&network_event.data[5], "Coupons Used", true);
  appd_iot_data_set_integer(&network_event.data[6], "Coupon Discount Percent", 20);

  retcode = appd_iot_add_network_request_event(network_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

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

  appd_iot_set_response_code(202);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test if callbacks are triggered
  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  appd_iot_clear_http_cb_triggered_flags();

  free(resp_headers);

  free(network_event.data);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}

/**
 * @brief Unit Test for valid minimal network event
 */
Ensure(network_event, returns_success_on_minimal_appd_iot_add_and_send_network_event)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "Point of Sale";
  devcfg.device_name = "POS#12";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_network_request_event_t network_event_1;
  appd_iot_network_request_event_t network_event_2;
  appd_iot_network_request_event_t network_event_3;

  appd_iot_init_to_zero(&network_event_1, sizeof(appd_iot_network_request_event_t));
  appd_iot_init_to_zero(&network_event_2, sizeof(appd_iot_network_request_event_t));
  appd_iot_init_to_zero(&network_event_3, sizeof(appd_iot_network_request_event_t));

  network_event_1.url = "http://apdy.com/processPayment";
  network_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);
  network_event_1.resp_code = 202;

  network_event_2.url = "http://apdy.com/updateInventory";
  network_event_2.timestamp_ms = ((int64_t)time(NULL) * 1000);
  network_event_2.error = appd_iot_error_code_to_str(APPD_IOT_ERR_NETWORK_UNREACHABLE);

  network_event_3.url = "http://apdy.com/searchInventory";

  retcode = appd_iot_add_network_request_event(network_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_add_network_request_event(network_event_2);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_add_network_request_event(network_event_3);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

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

  appd_iot_set_response_code(202);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test if callbacks are triggered
  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  appd_iot_clear_http_cb_triggered_flags();

  free(resp_headers);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}

/**
 * @brief Unit Test for null network event fields
 */
Ensure(network_event, returns_error_on_null_appd_iot_add_network_event)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "Point of Sale";
  devcfg.device_name = "POS#12";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_network_request_event_t network_event;

  appd_iot_init_to_zero(&network_event, sizeof(appd_iot_network_request_event_t));

  //check for null url
  retcode = appd_iot_add_network_request_event(network_event);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_INVALID_INPUT));

  //check for null resp code and null error
  network_event.url = "http://adpy.com/processPayment";
  network_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  retcode = appd_iot_add_network_request_event(network_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for invalid resp code and null error
  network_event.resp_code = 1024;
  retcode = appd_iot_add_network_request_event(network_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //check for network event with URL same as Collector URL
  network_event.url = TEST_EUM_COLLECTOR_URL;
  network_event.resp_code = 202;
  retcode = appd_iot_add_network_request_event(network_event);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_NOT_SUPPORTED));

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

  appd_iot_set_response_code(202);
  appd_iot_set_response_headers(resp_headers_count, resp_headers);
  appd_iot_clear_http_cb_triggered_flags();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test if callbacks are triggered
  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  appd_iot_clear_http_cb_triggered_flags();

  free(resp_headers);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for network event with BT Headers included
 */
Ensure(network_event, returns_success_on_appd_iot_add_network_event_with_bt)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "5555";
  devcfg.device_type = "Point of Sale";
  devcfg.device_name = "POS#12";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_network_request_event_t network_event;

  appd_iot_init_to_zero(&network_event, sizeof(appd_iot_network_request_event_t));

  network_event.url = "http://apdy.com/processPayment";
  network_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  network_event.resp_code = 202;

  network_event.resp_headers_count = 4;
  network_event.resp_headers = (appd_iot_data_t*)calloc(network_event.resp_headers_count,
                               sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&network_event.resp_headers[0], "ADRUM_0", TEST_ADRUM_0);
  appd_iot_data_set_string(&network_event.resp_headers[1], "ADRUM_1", TEST_ADRUM_1);
  appd_iot_data_set_string(&network_event.resp_headers[2], "ADRUM_2", TEST_ADRUM_2);
  appd_iot_data_set_string(&network_event.resp_headers[3], "ADRUM_3", TEST_ADRUM_3);

  retcode = appd_iot_add_network_request_event(network_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &appd_iot_test_http_req_send_cb;
  http_cb.http_resp_done_cb = &appd_iot_test_http_resp_done_cb;

  retcode = appd_iot_register_network_interface(http_cb);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_set_response_code(202);
  appd_iot_clear_http_cb_triggered_flags();
  appd_iot_clear_http_req_bt_headers_present_flag();

  retcode = appd_iot_send_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test if callbacks are triggered
  assert_that(appd_iot_is_http_req_send_cb_triggered(), is_equal_to(true));

  assert_that(appd_iot_is_http_resp_done_cb_triggered(), is_equal_to(true));

  //check if bt headers are present
  assert_that(appd_iot_is_http_req_bt_headers_present(), is_equal_to(true));


  appd_iot_clear_http_cb_triggered_flags();
  appd_iot_clear_http_req_bt_headers_present_flag();

  free(network_event.resp_headers);
}

TestSuite* network_event_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, network_event, returns_success_on_full_appd_iot_add_and_send_network_event);
  add_test_with_context(suite, network_event, returns_success_on_minimal_appd_iot_add_and_send_network_event);
  add_test_with_context(suite, network_event, returns_error_on_null_appd_iot_add_network_event);
  add_test_with_context(suite, network_event, returns_success_on_appd_iot_add_network_event_with_bt);

  return suite;
}

