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

#include <stdio.h>
#include <time.h>
#include <appd_iot_interface.h>
#include <unistd.h>
#include "http_curl_interface.hpp"
#include "log.hpp"
#include "custom_event.hpp"
#include "network_event.hpp"
#include "error_event.hpp"
#include "options.hpp"

static appd_iot_sdk_state_t sdk_state = APPD_IOT_SDK_UNINITIALIZED;

/**
 * @brief Callback function triggered by AppDynamics IoT SDK whenever SDK state changes.
 * Register this Callback function as part of SDK Init API (appd_iot_init_sdk())
 */
static void sdk_state_change_cb(appd_iot_sdk_state_t new_state)
{
  sdk_state = new_state;

  fprintf(stdout, "\nNew SDK State:%s\n", appd_iot_sdk_state_to_str(sdk_state));

  /**
   * If SDK gets disabled by Collector, implement an Asynchronous Timer Functionality here
   * to periodically call the API appd_iot_check_app_status() to check app status and re-enable sdk.
   */
}

/**
 * @brief Main for Sample App <br>
 * Initialize IoT CPP SDK, Send Custom, Network and Error Events
 * @param argc Number of Arguments supplied
 * @param argv Array of Arguments
 * @return int containing function execution status
 */
int main(int argc, char* argv[])
{
  if (read_options(argc, argv) == false)
  {
    free_options();
    return 1;
  }

  //Step1: initialize sdk
  appd_iot_sdk_config_t sdkcfg;
  appd_iot_device_config_t devcfg;
  appd_iot_error_code_t errcode;

  appd_iot_init_to_zero(&sdkcfg, sizeof(sdkcfg));
  appd_iot_init_to_zero(&devcfg, sizeof(devcfg));

  sdkcfg.appkey = get_app_key();
  sdkcfg.eum_collector_url = get_collector_url();
  sdkcfg.log_level = get_log_level();

  //If set to NULL and log level is not OFF, logs will be written to stderr
  sdkcfg.log_write_cb = &log_write_cb;

  //To get notified whenever SDK state changes
  sdkcfg.sdk_state_change_cb = &sdk_state_change_cb;

  devcfg.device_id = "1111";
  devcfg.device_type = "SmartCar";
  devcfg.device_name = "AudiS3";
  devcfg.fw_version = "1.0";
  devcfg.hw_version = "1.0";
  devcfg.os_version = "1.0";
  devcfg.sw_version = "1.0";

  //open log before initializing sdk to start log capture
  open_log(get_log_file());

  errcode = appd_iot_init_sdk(sdkcfg, devcfg);

  if (errcode != APPD_IOT_SUCCESS)
  {
    fprintf(stderr, "Error initializing sdk:%d\n", errcode);
    free_options();
    close_log();
    return 1;
  }

  //Step2: register http interface callbacks
  appd_iot_http_cb_t http_cb;
  http_cb.http_req_send_cb = &http_curl_req_send_cb;
  http_cb.http_resp_done_cb = &http_curl_resp_done_cb;

  errcode = appd_iot_register_network_interface(http_cb);

  if (errcode != APPD_IOT_SUCCESS)
  {
    fprintf(stderr, "Error Registering for network interface:%d\n", errcode);
    free_options();
    close_log();
    return 1;
  }

  event_type event = get_event_type();

  //Step3: send events
  if (event & CUSTOM_EVENT)
  {
    send_custom_event();
  }

  if (event & NETWORK_EVENT)
  {
    //trigger network request and capture event if url provided
    http_req_options_t* http_req_options = get_http_req_options();

    if (http_req_options->url != NULL)
    {
      fprintf(stdout, "Triggering Network Request to url:%s\n", http_req_options->url);
      capture_and_send_network_event(http_req_options->url, http_req_options->type, http_req_options->data);
    }
    //send sample network event
    else
    {
      send_network_event();
    }
  }

  if (event & ERROR_EVENT)
  {
    send_error_event_alert();
    send_error_event_critical();
    send_error_event_fatal();
  }

  /**
   * It is recommended to move below functionality into a separate thread
   * or use asynchronous timers instead of sleep. Below code is provided for reference to
   * periodically check with AppDynamics Collector if SDK can be enabled.
   */
  if ((sdk_state == APPD_IOT_SDK_DISABLED_KILL_SWITCH) ||
      (sdk_state == APPD_IOT_SDK_DISABLED_LICENSE_EXPIRED) ||
      (sdk_state == APPD_IOT_SDK_DISABLED_DATA_LIMIT_EXCEEDED))
  {
    int t = get_timer_value_in_sec();
    int r = get_num_retries();

    while (r > 0)
    {
      errcode = appd_iot_check_app_status();

      if ((errcode == APPD_IOT_SUCCESS) && (sdk_state == APPD_IOT_SDK_ENABLED))
      {
        fprintf(stdout, "SDK Enabled Successfully\n");
        break;
      }

      sleep (t);
      r--;
    }
  }

  free_options();
  close_log();

  return 0;
}
