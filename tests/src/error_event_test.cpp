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

Describe(error_event);
BeforeEach(error_event) { }
AfterEach(error_event) { }

/**
 * @brief Unit Test for valid alert and critical error event
 */
Ensure(error_event, test_full_alert_and_critical_error_event)
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

  appd_iot_error_event_t error_event_1, error_event_2;

  appd_iot_init_to_zero(&error_event_1, sizeof(appd_iot_error_event_t));
  appd_iot_init_to_zero(&error_event_2, sizeof(appd_iot_error_event_t));


  error_event_1.name = "Warning Light";
  error_event_1.message = "Oil Change Reminder";
  error_event_1.severity = APPD_IOT_ERR_SEVERITY_ALERT;
  error_event_1.timestamp_ms = ((int64_t)time(NULL) * 1000);
  error_event_1.duration_ms = 0;

  error_event_1.data_count = 1;
  error_event_1.data = (appd_iot_data_t*)calloc(error_event_1.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_integer(&error_event_1.data[0], "Mileage", 27300);

  retcode = appd_iot_add_error_event(error_event_1);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  error_event_2.name = "Bluetooth Connection Error";
  error_event_2.message = "connection dropped during voice call due to bluetooth exception";
  error_event_2.severity = APPD_IOT_ERR_SEVERITY_CRITICAL;
  error_event_2.timestamp_ms = ((int64_t)time(NULL) * 1000);
  error_event_2.duration_ms = 0;

  error_event_2.data_count = 3;
  error_event_2.data = (appd_iot_data_t*)calloc(error_event_2.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&error_event_2.data[0], "UUID", "00001101-0000-1000-8000-00805f9b34fb");
  appd_iot_data_set_string(&error_event_2.data[1], "Bluetooth Version", "3.0");
  appd_iot_data_set_integer(&error_event_2.data[2], "Error Code", 43);

  retcode = appd_iot_add_error_event(error_event_2);
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

  free(error_event_1.data);
  free(error_event_2.data);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for minimal alert and critical error event
 */
Ensure(error_event, test_minimal_alert_and_critical_error_event)
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

  appd_iot_error_event_t error_event;

  appd_iot_init_to_zero(&error_event, sizeof(appd_iot_error_event_t));

  error_event.name = "Tire Pressure Low";
  error_event.timestamp_ms = ((int64_t)time(NULL) * 1000);

  retcode = appd_iot_add_error_event(error_event);
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
 * @brief Unit Test for valid fatal error event
 */
Ensure(error_event, test_full_fatal_error_event)
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

  appd_iot_error_event_t error_event;
  appd_iot_init_to_zero(&error_event, sizeof(appd_iot_error_event_t));

  error_event.name = "I/O Exception";
  error_event.message = "error while writing data to file";
  error_event.severity = APPD_IOT_ERR_SEVERITY_FATAL;
  error_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  error_event.duration_ms = 0;
  error_event.stack_trace_count = 1;
  error_event.error_stack_trace_index = 0;

  appd_iot_stack_trace_t* stack_trace = (appd_iot_stack_trace_t*)calloc(error_event.stack_trace_count,
                                        sizeof(appd_iot_stack_trace_t));
  stack_trace->stack_frame_count = 4;
  stack_trace->thread = "main";
  appd_iot_stack_frame_t* stack_frame = (appd_iot_stack_frame_t*)calloc(stack_trace->stack_frame_count,
                                        sizeof(appd_iot_stack_frame_t));
  stack_trace->stack_frame = stack_frame;

  stack_frame[0].symbol_name = "_libc_start_main";
  stack_frame[0].package_name = "/system/lib/libc.so";

  stack_frame[1].symbol_name = "main";
  stack_frame[1].package_name = "/home/native-app/mediaplayer/build/mediaplayer_main.so";
  stack_frame[1].absolute_addr = 0x7f8bd984876c;
  stack_frame[1].image_offset = 18861;
  stack_frame[1].symbol_offset = 10;
  stack_frame[1].file_name = "main.c";
  stack_frame[1].lineno = 71;

  stack_frame[2].symbol_name = "write_data";
  stack_frame[2].package_name = "/home/native-app/mediaplayer/build/mediaplayer_main.so";
  stack_frame[2].absolute_addr = 0x7f8bda3f915b;
  stack_frame[2].image_offset = 116437;
  stack_frame[2].symbol_offset = 12;
  stack_frame[2].file_name = "writedata.c";
  stack_frame[2].lineno = 271;

  stack_frame[3].symbol_name = "write_to_file";
  stack_frame[3].package_name = "/home/native-app/mediaplayer/build/mediaplayer_main.so";
  stack_frame[3].absolute_addr = 0x7f8bda9f69d1;
  stack_frame[3].image_offset = 287531;
  stack_frame[3].symbol_offset = 34;
  stack_frame[3].file_name = "writedata.c";
  stack_frame[3].lineno = 524;

  error_event.stack_trace = stack_trace;

  error_event.data_count = 4;
  error_event.data = (appd_iot_data_t*)calloc(error_event.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&error_event.data[0], "filename", "buffer-126543.mp3");
  appd_iot_data_set_integer(&error_event.data[1], "filesize", 120000000);
  appd_iot_data_set_integer(&error_event.data[2], "signal number", 6);
  appd_iot_data_set_string(&error_event.data[3], "mediaplayer_version", "1.1");

  retcode = appd_iot_add_error_event(error_event);
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

  free(error_event.stack_trace->stack_frame);
  free(error_event.stack_trace);
  free(error_event.data);

  //test for log write cb
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for null error event
 */
Ensure(error_event, test_null_error_event)
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

  appd_iot_error_event_t error_event;

  appd_iot_init_to_zero(&error_event, sizeof(appd_iot_error_event_t));

  //null error event name
  retcode = appd_iot_add_error_event(error_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //zero stack traces
  error_event.name = "SIGINT1";
  error_event.stack_trace_count = 0;
  retcode = appd_iot_add_error_event(error_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //null stack traces
  error_event.name = "SIGINT2";
  error_event.stack_trace_count = 1;
  error_event.stack_trace = NULL;
  retcode = appd_iot_add_error_event(error_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  //null error stack frame
  error_event.name = "SIGINT3";
  error_event.error_stack_trace_index = 0;
  appd_iot_stack_trace_t* stack_trace = (appd_iot_stack_trace_t*)calloc(error_event.stack_trace_count,
                                        sizeof(appd_iot_stack_trace_t));
  error_event.stack_trace = stack_trace;
  stack_trace->stack_frame_count = 1;
  stack_trace->stack_frame = NULL;
  stack_trace->thread = "main";
  retcode = appd_iot_add_error_event(error_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

//zero error stack frame count
  error_event.name = "SIGINT4";
  stack_trace->stack_frame_count = 0;
  stack_trace->thread = "libc";
  retcode = appd_iot_add_error_event(error_event);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));

  free(stack_trace);

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

TestSuite* error_event_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, error_event, test_full_alert_and_critical_error_event);
  add_test_with_context(suite, error_event, test_minimal_alert_and_critical_error_event);
  add_test_with_context(suite, error_event, test_full_fatal_error_event);
  add_test_with_context(suite, error_event, test_null_error_event);

  return suite;
}




