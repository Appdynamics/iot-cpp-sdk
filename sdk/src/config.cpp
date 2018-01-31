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

#include "config.hpp"
#include "beacon.hpp"
#include "log.hpp"

static appd_sdk_config_t global_sdk_config;

static std::string appd_iot_eum_collector_url_appkey_prefix = "eumcollector/iot/v1/application/";
static std::string appd_iot_eum_collector_url_beacons_suffix = "/beacons";
static std::string appd_iot_eum_collector_url_enabled_suffix = "/enabled";
static std::string default_eum_collector_url = "https://iot-col.eum-appdynamics.com";

//Server Correlation headers in {key,value} format
static const appd_iot_data_t global_correlation_headers[APPD_IOT_NUM_SERVER_CORRELATION_HEADERS] =
{
  {"ADRUM", {"isAjax:true"}, APPD_IOT_STRING},
  {"ADRUM_1", {"isMobile:true"}, APPD_IOT_STRING}
};

static appd_iot_sdk_state_t global_sdk_state = APPD_IOT_SDK_UNINITIALIZED;

/**
 * @brief This method Initializes the SDK. <br>
 * This method should be called atleast once, early in your application's start up sequence.
 * @param sdkcfg contains sdk configuration such as collector url, appkey etc
 * @param devcfg contains device information such as device type/name and hw/fw/sw versions
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_init_sdk(appd_iot_sdk_config_t sdkcfg,
                                        appd_iot_device_config_t devcfg)
{
  std::string eum_collector_url;
  appd_iot_error_code_t retcode;

  /* Process Log Config */
  if (sdkcfg.log_level >= APPD_IOT_LOG_OFF && sdkcfg.log_level <= APPD_IOT_LOG_ALL)
  {
    global_sdk_config.log_level = sdkcfg.log_level;
  }
  else
  {
    global_sdk_config.log_level = APPD_IOT_LOG_ERROR;
  }

  if (global_sdk_config.log_level != APPD_IOT_LOG_OFF)
  {
    global_sdk_config.log_write_cb = sdkcfg.log_write_cb;
  }

  /* Process Device Config */
  retcode = appd_iot_init_device_config(devcfg);

  if (retcode != APPD_IOT_SUCCESS)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Device Config Initialization Failed");
    return retcode;
  }

  /* Process SDK Config */
  if (sdkcfg.appkey == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "AppKey cannot be NULL");
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  global_sdk_config.appkey = sdkcfg.appkey;

  if (sdkcfg.eum_collector_url == NULL)
  {
    eum_collector_url = default_eum_collector_url;
    appd_iot_log(APPD_IOT_LOG_ERROR, "EUM collector URL is NULL, setting to default:%s",
                 default_eum_collector_url.c_str());
  }
  else
  {
    eum_collector_url = sdkcfg.eum_collector_url;
  }

  if (eum_collector_url.at(eum_collector_url.length() - 1) != '/')
  {
    eum_collector_url = eum_collector_url + "/";
  }

  global_sdk_config.eum_collector_url =
    eum_collector_url + appd_iot_eum_collector_url_appkey_prefix +
    global_sdk_config.appkey + appd_iot_eum_collector_url_beacons_suffix;

  global_sdk_config.eum_appkey_enabled_url =
    eum_collector_url + appd_iot_eum_collector_url_appkey_prefix +
    global_sdk_config.appkey + appd_iot_eum_collector_url_enabled_suffix;

  appd_iot_log(APPD_IOT_LOG_INFO, "EUM Collector URL %s", global_sdk_config.eum_collector_url.c_str());


  if (sdkcfg.sdk_state_change_cb != NULL)
  {
    global_sdk_config.sdk_state_change_cb = sdkcfg.sdk_state_change_cb;
  }

  appd_iot_set_sdk_state(APPD_IOT_SDK_ENABLED);

  return APPD_IOT_SUCCESS;
}


/**
 * @brief This method registers network interface <br>
 * This method should be called atleast once, early in your application's start up sequence.
 * @param http_cb contains function pointers for http request send and http response done callbacks <br>.
 * http request send callback will be called when appd_iot_send_all_events is triggered. <br>
 * http response done callback will be called after send http request callback returns and
 * response is processed.
 * @return appd_iot_error_code_t Error code indicating if the function call is a success or fail.
 * Error code returned provides more details on the type of error occurred.
 */
appd_iot_error_code_t appd_iot_register_network_interface(appd_iot_http_cb_t http_cb)
{
  if (http_cb.http_req_send_cb == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "http_req_send_cb is NULL");
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  if (http_cb.http_resp_done_cb == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "http_resp_done_cb is NULL");
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  global_sdk_config.http_cb.http_req_send_cb = http_cb.http_req_send_cb;
  global_sdk_config.http_cb.http_resp_done_cb = http_cb.http_resp_done_cb;

  return APPD_IOT_SUCCESS;
}

/**
 * @brief Get http request send callback function pointer
 * @return callback function pointer
 */
appd_iot_http_req_send_cb_t appd_iot_get_http_req_send_cb(void)
{
  return global_sdk_config.http_cb.http_req_send_cb;
}

/**
 * @brief Get http response done callback function pointer
 * @return callback function pointer
 */
appd_iot_http_resp_done_cb_t appd_iot_get_http_resp_done_cb(void)
{
  return global_sdk_config.http_cb.http_resp_done_cb;
}

/**
  * @brief Get Log Level configured as part of SDK Initialization
  * @return appd_iot_log_level_t contains log level enum
  */
appd_iot_log_level_t appd_iot_get_log_level(void)
{
  return global_sdk_config.log_level;
}


/**
 * @brief Get Log Write Callback Function Pointer.
 * @return appd_iot_log_write_cb_t contains log_write_cb fun ptr
 */
appd_iot_log_write_cb_t appd_iot_get_log_write_cb(void)
{
  return global_sdk_config.log_write_cb;
}

/**
 * @brief Get Configured EUM Collector URL
 * @return URL in string format
 */
const char* appd_iot_get_eum_collector_url(void)
{
  return global_sdk_config.eum_collector_url.c_str();
}

/**
 * @brief Get Server Correlation headers in {key,value} format that need to be added to
 * every outgoing http request to enable capturing Business Transaction (BT).
 * @return Server Correlation Headers. Total number of headers present is given by
 * macro APPD_IOT_NUM_SERVER_CORRELATION_HEADERS. Do not modify or free returned headers.
 */
const appd_iot_data_t* appd_iot_get_server_correlation_headers(void)
{
  return global_correlation_headers;
}

/**
 * @brief Set SDK State
 * @param new_state indicates the new sdk state that need to be set
 */
void appd_iot_set_sdk_state(appd_iot_sdk_state_t new_state)
{
  if (global_sdk_state == new_state)
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "SDK state update with same state as current:%s",
                 appd_iot_sdk_state_to_str(global_sdk_state));
    return;
  }

  global_sdk_state = new_state;

  appd_iot_log(APPD_IOT_LOG_INFO, "New SDK state :%s", appd_iot_sdk_state_to_str(global_sdk_state));

  if (global_sdk_config.sdk_state_change_cb != NULL)
  {
    global_sdk_config.sdk_state_change_cb(global_sdk_state);
  }
}

/**
 * @brief Get Current SDK State
 * @return appd_iot_sdk_state_t with current sdk state
 */
appd_iot_sdk_state_t appd_iot_get_sdk_state(void)
{
  return global_sdk_state;
}

/**
 * @brief Set SDK state to disabled state based on the HTTP Response Code
 * @param http_resp_code indicates the response code from the Collector
 */
void appd_iot_disable_sdk(int http_resp_code)
{
  if (http_resp_code == 403)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Resp Code:%d Application on Controller is Disabled",
                 http_resp_code);
    appd_iot_set_sdk_state(APPD_IOT_SDK_DISABLED_KILL_SWITCH);
  }
  else if (http_resp_code == 429)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Resp Code:%d Application Data Limit Exceeded", http_resp_code);
    appd_iot_set_sdk_state(APPD_IOT_SDK_DISABLED_DATA_LIMIT_EXCEEDED);
  }
  else if (http_resp_code == 402)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Resp Code:%d Application License Expired", http_resp_code);
    appd_iot_set_sdk_state(APPD_IOT_SDK_DISABLED_LICENSE_EXPIRED);
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_INFO, "Resp Code:%d not supported to disable SDK", http_resp_code);
  }
}



/**
 * @brief Use this API to check with AppDynamics Collector on the status of IoT Application on
 * AppDynamics Controller, whether instrumentation is enabled or not. If the Collector returns Success, SDK
 * gets ENABLED in case it has been DISABLED previously by Collector due to license expiry, kill switch or
 * data limit exceeded. <br>
 * It is required that SDK initialization is already done using the API appd_iot_init_sdk() before
 * calling this function.
 * @return appd_iot_error_code_t will indicate success if app is enabled
 */
appd_iot_error_code_t appd_iot_check_app_status(void)
{
  appd_iot_sdk_state_t curr_sdk_state = appd_iot_get_sdk_state();

  if (curr_sdk_state == APPD_IOT_SDK_UNINITIALIZED)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "SDK is Uninitalized. Call Init SDK before calling this API");
    return APPD_IOT_ERR_SDK_NOT_ENABLED;
  }

  /* Init all the data structures - REQ and RESP */
  appd_iot_http_req_t http_req;
  appd_iot_http_resp_t* http_resp = NULL;
  appd_iot_error_code_t retcode = APPD_IOT_SUCCESS;
  appd_iot_http_req_send_cb_t http_req_send_cb = appd_iot_get_http_req_send_cb();
  appd_iot_http_resp_done_cb_t http_resp_done_cb = appd_iot_get_http_resp_done_cb();

  if (http_req_send_cb == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Network Interface Not Available");
    return APPD_IOT_ERR_NETWORK_NOT_AVAILABLE;
  }

  appd_iot_init_to_zero(&http_req, sizeof(http_req));

  http_req.type = "GET";
  http_req.url = global_sdk_config.eum_appkey_enabled_url.c_str();

  http_resp = http_req_send_cb(&http_req);

  /* check if any error present in http response */
  if (http_resp != NULL)
  {
    retcode = http_resp->error;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "NULL HTTP Response Returned");
    retcode = APPD_IOT_ERR_NULL_PTR;
  }

  /* Return if there is an error executing http req */
  if (retcode != APPD_IOT_SUCCESS)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Error Executing HTTP Request, ErrorCode:%d", retcode);

    if (http_resp_done_cb != NULL)
    {
      http_resp_done_cb(http_resp);
    }

    return retcode;
  }

  if (http_resp->resp_code >= 200 && http_resp->resp_code < 300)
  {
    appd_iot_set_sdk_state(APPD_IOT_SDK_ENABLED);
    appd_iot_log(APPD_IOT_LOG_INFO, "RespCode:%d Application is Enabled on Controller", http_resp->resp_code);
    retcode = APPD_IOT_SUCCESS;
  }
  else if ((http_resp->resp_code == 402) ||
           (http_resp->resp_code == 403) ||
           (http_resp->resp_code == 429))
  {
    appd_iot_disable_sdk(http_resp->resp_code);
    retcode = APPD_IOT_ERR_NETWORK_REJECT;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Resp Code:%d Network Request to Check App Status Failed",
                 http_resp->resp_code);
    retcode = APPD_IOT_ERR_NETWORK_ERROR;
  }

  if (http_resp_done_cb != NULL)
  {
    http_resp_done_cb(http_resp);
  }

  return retcode;
}
