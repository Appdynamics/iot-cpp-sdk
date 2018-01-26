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
#include "common_test.hpp"
#include "log_mock_interface.hpp"

using namespace cgreen;

Describe(log_interface);
BeforeEach(log_interface) { }
AfterEach(log_interface) { }

/**
 * @brief Unit Test for valid sdk config with log level set to ALL
 */
Ensure(log_interface, returns_success_on_valid_appd_iot_config_with_log_all)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;

  devcfg.device_id = "1234";
  devcfg.device_type = "Thermostat";
  devcfg.device_name = "Thermostat-A";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  //test for logs with log level set to ALL.
  sdkcfg.log_level = APPD_IOT_LOG_ALL;
  appd_iot_clear_log_write_cb_flags();
  appd_iot_error_code_t retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}

/**
 * @brief Unit Test for invalid sdk config with log level set to ERROR
 */
Ensure(log_interface, returns_success_on_invalid_appd_iot_sdk_config)
{

  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = NULL;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;

  devcfg.device_id = "1234";
  devcfg.device_type = "Thermostat";
  devcfg.device_name = "Thermostat-A";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  //test for error log with log level set to error.
  sdkcfg.log_level = APPD_IOT_LOG_ERROR;
  appd_iot_clear_log_write_cb_flags();
  appd_iot_error_code_t retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_INVALID_INPUT));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));

  //test for error log with log level set to warning.
  sdkcfg.log_level = APPD_IOT_LOG_WARN;
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_INVALID_INPUT));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for invalid log configuration
 */
Ensure(log_interface, returns_fail_on_invalid_appd_iot_log_config)
{

  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;

  devcfg.device_id = "1234";
  devcfg.device_type = "Thermostat";
  devcfg.device_name = "Thermostat-A";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  //log level is ERROR, test for NULL log write cb
  sdkcfg.log_level = APPD_IOT_LOG_ERROR;
  sdkcfg.log_write_cb = NULL;
  appd_iot_clear_log_write_cb_flags();
  appd_iot_error_code_t retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(false));


  //log level is off, test for NULL log write cb
  sdkcfg.log_level = APPD_IOT_LOG_OFF;
  sdkcfg.log_write_cb = NULL;
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(false));


  //log level is off, test for valid log write cb
  sdkcfg.log_level = APPD_IOT_LOG_OFF;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;;
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(false));
}

TestSuite* log_interface_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, log_interface, returns_success_on_valid_appd_iot_config_with_log_all);
  add_test_with_context(suite, log_interface, returns_success_on_invalid_appd_iot_sdk_config);
  add_test_with_context(suite, log_interface, returns_fail_on_invalid_appd_iot_log_config);

  return suite;
}
