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
#include "common_test.hpp"
#include "log_mock_interface.hpp"

using namespace cgreen;

Describe(config);
BeforeEach(config) { }
AfterEach(config) { }

/**
 * @brief Unit Test for Valid Full Config
 */
Ensure(config, returns_success_on_full_appd_iot_config)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "1234";
  devcfg.device_type = "Thermostat";
  devcfg.device_name = "Thermostat-A";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  appd_iot_clear_log_write_cb_flags();
  appd_iot_error_code_t retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}

/**
 * @brief Unit Test for Valid Minimal Config
 */
Ensure(config, returns_success_on_minimal_appd_iot_config)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ALL;

  devcfg.device_id = "1234";
  devcfg.device_type = "Thermostat";
  devcfg.device_name = NULL;
  devcfg.fw_version = NULL;
  devcfg.hw_version = NULL;
  devcfg.os_version = NULL;
  devcfg.sw_version = NULL;

  appd_iot_clear_log_write_cb_flags();
  appd_iot_error_code_t retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));

  //collector url is null, test for default collector url
  appd_iot_clear_log_write_cb_flags();
  sdkcfg.eum_collector_url = NULL;
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for NULL SDK Config
 */
Ensure(config, returns_error_on_null_appd_iot_sdk_config)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode = APPD_IOT_ERR_INVALID_INPUT;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_ERROR;

  devcfg.device_id = "1234";
  devcfg.device_type = "Thermostat";
  devcfg.device_name = NULL;
  devcfg.fw_version = NULL;
  devcfg.hw_version = NULL;
  devcfg.os_version = NULL;
  devcfg.sw_version = NULL;

  //appkey is null
  sdkcfg.appkey = NULL;
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_INVALID_INPUT));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}


/**
 * @brief Unit Test for NULL DEV Config
 */
Ensure(config, returns_error_on_null_appd_iot_dev_config)
{
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));
  appd_iot_init_to_zero(&devcfg, sizeof(devcfg));

  sdkcfg.appkey = TEST_APP_KEY;
  sdkcfg.eum_collector_url = TEST_EUM_COLLECTOR_URL;
  sdkcfg.log_write_cb = &appd_iot_log_write_cb;
  sdkcfg.log_level = APPD_IOT_LOG_WARN;

  //device_id is null
  devcfg.device_id = NULL;
  devcfg.device_type = "Thermostat";
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_SUCCESS));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));

  //device_type is null
  devcfg.device_id = "1234";
  devcfg.device_type = NULL;
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_INVALID_INPUT));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));

  //both device_id and device_type are null
  devcfg.device_id = NULL;
  appd_iot_clear_log_write_cb_flags();
  retcode = appd_iot_init_sdk(sdkcfg, devcfg);
  assert_that(retcode, is_equal_to(APPD_IOT_ERR_INVALID_INPUT));
  assert_that(appd_iot_is_log_write_cb_success(), is_equal_to(true));
}

TestSuite* config_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, config, returns_success_on_full_appd_iot_config);
  add_test_with_context(suite, config, returns_success_on_minimal_appd_iot_config);
  add_test_with_context(suite, config, returns_error_on_null_appd_iot_sdk_config);
  add_test_with_context(suite, config, returns_error_on_null_appd_iot_dev_config);

  return suite;
}
