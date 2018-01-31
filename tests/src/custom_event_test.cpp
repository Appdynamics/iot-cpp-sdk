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

#include <cgreen/cgreen.h>
#include <appd_iot_interface.h>
#include <time.h>
#include <unistd.h>
#include "common_test.hpp"
#include "http_mock_interface.hpp"
#include "log_mock_interface.hpp"

using namespace cgreen;

Describe(custom_event);
BeforeEach(custom_event) { }
AfterEach(custom_event) { }

/**
 * @brief Unit Test for valid full custom event
 */
Ensure(custom_event, returns_success_on_full_appd_iot_add_and_send_custom_event)
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
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_custom_event_t custom_event_1, custom_event_2;
  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000); //in millisecond epoch time
  custom_event_1.duration_ms = 10;
  custom_event_1.data_count = 6;
  custom_event_1.data = (appd_iot_data_t*)calloc(custom_event_1.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&custom_event_1.data[0], "VinNumber", "VN01234");
  appd_iot_data_set_integer(&custom_event_1.data[1], "MPG Reading", 23);
  appd_iot_data_set_integer(&custom_event_1.data[2], "Annual Mileage", 12000);
  appd_iot_data_set_double(&custom_event_1.data[3], "Temperature", 101.3);
  appd_iot_data_set_boolean(&custom_event_1.data[4], "Engine Lights ON", false);

  int64_t engine_start_time_1 = ((int64_t)time(NULL) * 1000);
  appd_iot_data_set_datetime(&custom_event_1.data[5], "Last Engine Start Time", engine_start_time_1);

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  custom_event_2.type = "Smart Car Reading 2";
  custom_event_2.summary = "Events Captured in Smart Car 2";
  custom_event_2.timestamp_ms = ((int64_t)time(NULL) * 1000); //in millisecond epoch time
  custom_event_2.duration_ms = 30;
  custom_event_2.data_count = 6;
  custom_event_2.data = (appd_iot_data_t*)calloc(custom_event_2.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&custom_event_2.data[0], "VinNumber", "VN56789");
  appd_iot_data_set_integer(&custom_event_2.data[1], "MPG Reading", 36);
  appd_iot_data_set_integer(&custom_event_2.data[2], "Annual Mileage", 24000);
  appd_iot_data_set_double(&custom_event_2.data[3], "Temperature", 98.7);
  appd_iot_data_set_boolean(&custom_event_2.data[4], "Engine Lights ON", true);

  int64_t engine_start_time_2 = ((int64_t)time(NULL) * 1000);
  appd_iot_data_set_datetime(&custom_event_2.data[5], "Last Engine Start Time", engine_start_time_2);

  retcode = appd_iot_add_custom_event(custom_event_2);
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
  free(custom_event_1.data);
  free(custom_event_2.data);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for valid minimal custom event
 */
Ensure(custom_event, returns_success_on_minimal_appd_iot_add_and_send_custom_event)
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
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_custom_event_t custom_event_1, custom_event_2;

  appd_iot_init_to_zero(&custom_event_1, sizeof(appd_iot_custom_event_t));
  appd_iot_init_to_zero(&custom_event_2, sizeof(appd_iot_custom_event_t));

  custom_event_1.type = "Smart Car Reading 1";
  custom_event_1.summary = "Events Captured in Smart Car 1";
  custom_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);

  custom_event_2.type = "Smart Car Reading 2";

  retcode = appd_iot_add_custom_event(custom_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  retcode = appd_iot_add_custom_event(custom_event_2);
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

  free(custom_event_1.data);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}

/**
 * @brief Unit Test for valid minimal device config
 */
Ensure(custom_event, test_minimal_device_config)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));
  appd_iot_init_to_zero(&devcfg, sizeof(devcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_type = "SmartCar";

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
 * @brief Unit Test for NULL custom event fields
 */
Ensure(custom_event, check_for_null_fields_appd_iot_add_custom_event)
{

  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "1111";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  appd_iot_custom_event_t custom_event;
  appd_iot_init_to_zero(&custom_event, sizeof(custom_event));

  custom_event.type = "Smart Car";
  custom_event.summary = "Events Captured in Smart Car";
  custom_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  custom_event.duration_ms = 0;
  custom_event.data_count = 1;

  //test for event type = NULL
  custom_event.type = NULL;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for event type with pipe character
  custom_event.type = "Smart|Car";
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for event summary = NULL
  custom_event.type = "Smart Car";
  custom_event.summary  = NULL;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for event timestamp_ms = 0
  custom_event.summary = "Events Captured in Smart Car";
  custom_event.timestamp_ms = 0;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for event timestamp_ms = 0, type and summary = NULL
  custom_event.type = NULL;
  custom_event.summary  = NULL;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for event data = NULL
  custom_event.type = "Smart Car";
  custom_event.summary = "Events Captured in Smart Car";
  custom_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  custom_event.data = NULL;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for  event key = NULL
  custom_event.data = (appd_iot_data_t*)calloc(custom_event.data_count, sizeof(appd_iot_data_t));
  custom_event.data[0].key = NULL;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for NULL String value = NULL
  appd_iot_data_set_string(&custom_event.data[0], "VinNumber2", NULL);
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for invalid event value type
  appd_iot_data_set_string(&custom_event.data[0], "VinNumber", "VN123456");
  custom_event.data[0].value_type = (appd_iot_data_types_t)12345;
  retcode = appd_iot_add_custom_event(custom_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //test for invalid event key
  appd_iot_data_set_string(&custom_event.data[0], "Vin|Number", "VN123456");
  retcode = appd_iot_add_custom_event(custom_event);
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

  //test for clearing empty events
  retcode = appd_iot_clear_all_events();
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  free(custom_event.data);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


TestSuite* custom_event_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, custom_event, returns_success_on_full_appd_iot_add_and_send_custom_event);
  add_test_with_context(suite, custom_event, returns_success_on_minimal_appd_iot_add_and_send_custom_event);
  add_test_with_context(suite, custom_event, test_minimal_device_config);
  add_test_with_context(suite, custom_event, check_for_null_fields_appd_iot_add_custom_event);

  return suite;
}
