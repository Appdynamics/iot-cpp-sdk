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

#include <string.h>
#include <stdlib.h>
#include "beacon.hpp"
#include "log.hpp"
#include "json_serializer.hpp"
#include "config.hpp"
#include "utils.hpp"

#define APPD_IOT_SDK_VERSION "APPD_IOT_CPP_SDK_1.0.1"

static beacon_t global_beacon;

static std::string appd_iot_serialize_beacon_to_json(beacon_t beacon);

/**
 * @brief Initializes Device Configuration <br>
 * It is madatory to set Device ID and Device Type.
 * @param devcfg contains device configuration
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_init_device_config(appd_iot_device_config_t devcfg)
{

  if (devcfg.device_type == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Device Type cannot be NULL");
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  global_beacon.devcfg.device_type = appd_iot_remove_character(devcfg.device_type, '|');

  if (devcfg.device_id != NULL)
  {
    global_beacon.devcfg.device_id = devcfg.device_id;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Device ID is NULL");
  }

  if (devcfg.device_name != NULL)
  {
    global_beacon.devcfg.device_name = devcfg.device_name;
  }

  if (devcfg.fw_version != NULL)
  {
    global_beacon.devcfg.fw_version = devcfg.fw_version;
  }

  if (devcfg.hw_version != NULL)
  {
    global_beacon.devcfg.hw_version = devcfg.hw_version;
  }

  if (devcfg.os_version != NULL)
  {
    global_beacon.devcfg.os_version = devcfg.os_version;
  }

  if (devcfg.sw_version != NULL)
  {
    global_beacon.devcfg.sw_version = devcfg.sw_version;
  }

  return APPD_IOT_SUCCESS;
}

/**
  * @brief Adds Custom Event to Beacon
  * @param event contains custom event data to be sent to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_custom_event_to_beacon(custom_event_t event)
{
  if (global_beacon.custom_event_list.size() < APPD_IOT_MAX_CUSTOM_EVENTS)
  {
    global_beacon.custom_event_list.push_back(event);

    appd_iot_log(APPD_IOT_LOG_INFO, "Custom Event Added, Size:%lu",
                 (unsigned long)global_beacon.custom_event_list.size());

    return APPD_IOT_SUCCESS;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_ERROR,
                 "Max Custom Events (%d) in Buffer. Send Events in Buffer to Collector before adding new events",
                 APPD_IOT_MAX_CUSTOM_EVENTS);

    return APPD_IOT_ERR_MAX_LIMIT;
  }
}

/**
  * @brief Adds Network Request Event to Beacon
  * @param event contains network request event data to be sent to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_network_request_event_to_beacon(network_request_event_t event)
{
  if (global_beacon.network_request_event_list.size() < APPD_IOT_MAX_NETWORK_EVENTS)
  {
    global_beacon.network_request_event_list.push_back(event);

    appd_iot_log(APPD_IOT_LOG_INFO, "Network Event Added, Size:%lu",
                 (unsigned long)global_beacon.network_request_event_list.size());

    return APPD_IOT_SUCCESS;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_ERROR,
                 "Max Network Events (%d) in Buffer. Send Events in Buffer to Collector before adding new events",
                 APPD_IOT_MAX_NETWORK_EVENTS);

    return APPD_IOT_ERR_MAX_LIMIT;
  }
}

/**
  * @brief Adds Error Event to Beacon
  * @param event contains error event data to be sent to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_error_event_to_beacon(error_event_t event)
{
  if (global_beacon.error_event_list.size() < APPD_IOT_MAX_ERROR_EVENTS)
  {
    global_beacon.error_event_list.push_back(event);

    appd_iot_log(APPD_IOT_LOG_INFO, "Error Event Added, Size:%lu",
                 (unsigned long)global_beacon.error_event_list.size());

    return APPD_IOT_SUCCESS;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_ERROR,
                 "Max Error Events (%d) in Buffer. Send Events in Buffer to Collector before adding new events",
                 APPD_IOT_MAX_ERROR_EVENTS);

    return APPD_IOT_ERR_MAX_LIMIT;
  }

}


/**
  * @brief Clears Beacons in memory
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_clear_all_beacons(void)
{
  appd_iot_log(APPD_IOT_LOG_INFO, "Clearing All Beacons");
  appd_iot_log(APPD_IOT_LOG_INFO, "Clearing %lu Custom Events",
               (unsigned long)global_beacon.custom_event_list.size());
  appd_iot_log(APPD_IOT_LOG_INFO, "Clearing %lu Network Events",
               (unsigned long)global_beacon.network_request_event_list.size());
  appd_iot_log(APPD_IOT_LOG_INFO, "Clearing %lu Error Events",
               (unsigned long)global_beacon.error_event_list.size());

  global_beacon.custom_event_list.clear();
  global_beacon.network_request_event_list.clear();
  global_beacon.error_event_list.clear();

  return APPD_IOT_SUCCESS;
}


/**
  * @brief Sends Beacons in memory to collector. <br>
  * Max Limit on the number of events in the beacon is defined by <br>
  * APPD_IOT_MAX_CUSTOM_EVENTS, APPD_IOT_MAX_NETWORK_EVENTS and APPD_IOT_MAX_CUSTOM_EVENTS
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_send_all_beacons(void)
{

  if (global_beacon.custom_event_list.size() == 0 &&
      global_beacon.network_request_event_list.size() == 0 &&
      global_beacon.error_event_list.size() == 0)
  {
    appd_iot_log(APPD_IOT_LOG_INFO, "No Events Present");
    return APPD_IOT_SUCCESS;
  }

  appd_iot_log(APPD_IOT_LOG_INFO, "Sending All Beacons");
  appd_iot_log(APPD_IOT_LOG_INFO, "Sending %lu Custom Events",
               (unsigned long)global_beacon.custom_event_list.size());
  appd_iot_log(APPD_IOT_LOG_INFO, "Sending %lu Network Events",
               (unsigned long)global_beacon.network_request_event_list.size());
  appd_iot_log(APPD_IOT_LOG_INFO, "Sending %lu Error Events",
               (unsigned long)global_beacon.error_event_list.size());

  /* Init all the data structures - REQ and RESP */
  appd_iot_http_req_t http_req;
  appd_iot_http_resp_t* http_resp = NULL;
  std::string jsondata;
  appd_iot_error_code_t retcode = APPD_IOT_SUCCESS;
  appd_iot_http_req_send_cb_t http_req_send_cb = appd_iot_get_http_req_send_cb();
  appd_iot_http_resp_done_cb_t http_resp_done_cb = appd_iot_get_http_resp_done_cb();

  if (http_req_send_cb == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Network Interface Not Available");
    return APPD_IOT_ERR_NETWORK_NOT_AVAILABLE;
  }

  jsondata = appd_iot_serialize_beacon_to_json(global_beacon);

  if (jsondata.empty())
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to Serialize Data to JSON Format");
    return APPD_IOT_ERR_NULL_PTR;
  }

  char jsonlen_buf[10];
  snprintf(jsonlen_buf, sizeof(jsonlen_buf), "%lu", (unsigned long)jsondata.length());

  appd_iot_init_to_zero(&http_req, sizeof(http_req));

  http_req.data = jsondata.c_str();
  http_req.type = "POST";
  http_req.url = appd_iot_get_eum_collector_url();
  http_req.headers_count = 3;
  http_req.headers = (appd_iot_data_t*)calloc(http_req.headers_count, sizeof(appd_iot_data_t));

  if (http_req.headers == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to Create HTTP Request Headers");
    return APPD_IOT_ERR_NULL_PTR;
  }

  appd_iot_data_set_string(&http_req.headers[0], "Accept", "application/json");
  appd_iot_data_set_string(&http_req.headers[1], "Content-Type", "application/json");
  appd_iot_data_set_string(&http_req.headers[2], "Content-Length", jsonlen_buf);

  appd_iot_log(APPD_IOT_LOG_INFO, "Content Len:%lu", (unsigned long)jsondata.length());

  http_resp = http_req_send_cb(&http_req);

  free(http_req.headers);

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
    appd_iot_log(APPD_IOT_LOG_ERROR, "Error Executing HTTP Request:%s",
                 appd_iot_error_code_to_str(retcode));

    if (http_resp_done_cb != NULL)
    {
      http_resp_done_cb(http_resp);
    }

    return retcode;
  }

  /* Read http response headers, content and response code */
  for (int i = 0; i < http_resp->headers_count; i++)
  {
    if ((http_resp->headers + i) == NULL)
    {
      continue;
    }

    if (http_resp->headers[i].key == NULL || http_resp->headers[i].strval == NULL ||
        http_resp->headers[i].value_type != APPD_IOT_STRING)
    {
      continue;
    }

    appd_iot_log(APPD_IOT_LOG_INFO, "Response Header%d (%s:%s)", i, http_resp->headers[i].key,
                 http_resp->headers[i].strval);
  }

  if (http_resp->content_len > 0)
  {
    appd_iot_log(APPD_IOT_LOG_INFO, "Response Content Len:%lu", (unsigned long)http_resp->content_len);
    appd_iot_log(APPD_IOT_LOG_INFO, "Response Content:%s", http_resp->content);
  }

  if (http_resp->resp_code >= 200 && http_resp->resp_code < 300)
  {
    appd_iot_log(APPD_IOT_LOG_INFO, "RespCode:%d Beacon Sent Successfully", http_resp->resp_code);
    appd_iot_clear_all_beacons();
    retcode = APPD_IOT_SUCCESS;
  }
  else if ((http_resp->resp_code == 402) ||
           (http_resp->resp_code == 403) ||
           (http_resp->resp_code == 429))
  {
    appd_iot_clear_all_beacons();
    appd_iot_disable_sdk(http_resp->resp_code);
    retcode = APPD_IOT_ERR_NETWORK_REJECT;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Resp Code:%d Send Beacons Network Request Failed", http_resp->resp_code);
    retcode = APPD_IOT_ERR_NETWORK_ERROR;
  }

  if (http_resp_done_cb != NULL)
  {
    http_resp_done_cb(http_resp);
  }

  return retcode;
}


/**
 * @brief Serializes Data into JSON Format
 * @param json object which contains buffer to which serialized data is written to
 * @param data contains data_t object from which data properties are read
 */
static void appd_iot_serialize_properties_data_to_json(json_t* json, data_t* data)
{
  if (json == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Serializing Properties failed due to NULL json Object");
    return;
  }

  if (data == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Serializing Properties failed due to NULL data Object");
    return;
  }

  if (!data->stringmap.empty())
  {
    appd_iot_json_start_object(json, "stringProperties");

    for (std::map<std::string, std::string>::iterator it = data->stringmap.begin();
         it != data->stringmap.end(); ++it)
    {
      std::string key = it->first;
      std::string value = it->second;
      appd_iot_json_add_string_key_value(json, key.c_str(), value.c_str());
    }

    appd_iot_json_end_object(json);
  }

  if (!data->integermap.empty())
  {
    appd_iot_json_start_object(json, "longProperties");

    for (std::map<std::string, int64_t>::iterator it = data->integermap.begin();
         it != data->integermap.end(); ++it)
    {
      std::string key = it->first;
      int64_t value = it->second;
      appd_iot_json_add_integer_key_value(json, key.c_str(), value);
    }

    appd_iot_json_end_object(json);
  }

  if (!data->doublemap.empty())
  {
    appd_iot_json_start_object(json, "doubleProperties");

    for (std::map<std::string, double>::iterator it = data->doublemap.begin();
         it != data->doublemap.end(); ++it)
    {
      std::string key = it->first;
      double value = it->second;
      appd_iot_json_add_double_key_value(json, key.c_str(), value);
    }

    appd_iot_json_end_object(json);
  }

  if (!data->boolmap.empty())
  {
    appd_iot_json_start_object(json, "booleanProperties");

    for (std::map<std::string, bool>::iterator it = data->boolmap.begin();
         it != data->boolmap.end(); ++it)
    {
      std::string key = it->first;
      bool value = it->second;
      appd_iot_json_add_boolean_key_value(json, key.c_str(), value);
    }

    appd_iot_json_end_object(json);
  }

  if (!data->datetimemap.empty())
  {
    appd_iot_json_start_object(json, "datetimeProperties");

    for (std::map<std::string, int64_t>::iterator it = data->datetimemap.begin();
         it != data->datetimemap.end(); ++it)
    {
      std::string key = it->first;
      int64_t value = it->second;
      appd_iot_json_add_integer_key_value(json, key.c_str(), value);
    }

    appd_iot_json_end_object(json);
  }
}


/**
  * @brief Serializes Beacon Data into JSON Format
  * @param beacon contains beacon data to be serialized
  * @return string which contains json formatted data
  */
static std::string appd_iot_serialize_beacon_to_json(beacon_t beacon)
{
  /* Initialize JSON */
  json_t* json = appd_iot_json_init();

  appd_iot_json_start_array(json, NULL);
  appd_iot_json_start_object(json, NULL);

  /* Set SDK Version */
  appd_iot_json_add_string_key_value(json, "agentVersion", APPD_IOT_SDK_VERSION);

  /* Start Device Config Processing */
  if (!(beacon.devcfg.device_id.empty() &&
        beacon.devcfg.device_name.empty() &&
        beacon.devcfg.device_type.empty()))
  {
    appd_iot_json_start_object(json, "deviceInfo");

    /* Start Device Config Processing */
    if (!beacon.devcfg.device_id.empty())
    {
      appd_iot_json_add_string_key_value(json, "deviceId", beacon.devcfg.device_id.c_str());
    }

    if (!beacon.devcfg.device_name.empty())
    {
      appd_iot_json_add_string_key_value(json, "deviceName", beacon.devcfg.device_name.c_str());
    }

    if (!beacon.devcfg.device_type.empty())
    {
      appd_iot_json_add_string_key_value(json, "deviceType", beacon.devcfg.device_type.c_str());
    }

    appd_iot_json_end_object(json);
  }

  if (!(beacon.devcfg.hw_version.empty() &&
        beacon.devcfg.fw_version.empty() &&
        beacon.devcfg.sw_version.empty() &&
        beacon.devcfg.os_version.empty()))
  {
    appd_iot_json_start_object(json, "versionInfo");

    if (!beacon.devcfg.hw_version.empty())
    {
      appd_iot_json_add_string_key_value(json, "hardwareVersion", beacon.devcfg.hw_version.c_str());
    }

    if (!beacon.devcfg.fw_version.empty())
    {
      appd_iot_json_add_string_key_value(json, "firmwareVersion", beacon.devcfg.fw_version.c_str());
    }

    if (!beacon.devcfg.sw_version.empty())
    {
      appd_iot_json_add_string_key_value(json, "softwareVersion", beacon.devcfg.sw_version.c_str());
    }

    if (!beacon.devcfg.os_version.empty())
    {
      appd_iot_json_add_string_key_value(json, "operatingSystemVersion", beacon.devcfg.os_version.c_str());
    }

    appd_iot_json_end_object(json);
  } /* End Device Config Processing */

  /* Start Custom Event Processing */
  if (beacon.custom_event_list.size() != 0)
  {
    std::list<custom_event_t>::iterator it = beacon.custom_event_list.begin();

    appd_iot_json_start_array(json, "customEvents");

    for (; it != beacon.custom_event_list.end(); ++it)
    {
      custom_event_t event = (custom_event_t)(*it);

      appd_iot_json_start_object(json, NULL);

      if (!event.type.empty())
      {
        appd_iot_json_add_string_key_value(json, "eventType", event.type.c_str());
      }

      if (!event.summary.empty())
      {
        appd_iot_json_add_string_key_value(json, "eventSummary", event.summary.c_str());
      }

      if (event.timestamp_ms != 0)
      {
        appd_iot_json_add_integer_key_value(json, "timestamp", event.timestamp_ms);
      }

      if (event.duration_ms > 0)
      {
        appd_iot_json_add_integer_key_value(json, "duration", event.duration_ms);
      }

      appd_iot_serialize_properties_data_to_json(json, &event.data);

      appd_iot_json_end_object(json);
    }

    appd_iot_json_end_array(json);
  } /* End Custom Event Processing */

  /* Start Network Event Processing */
  if (beacon.network_request_event_list.size() != 0)
  {
    std::list<network_request_event_t>::iterator it = beacon.network_request_event_list.begin();

    appd_iot_json_start_array(json, "networkRequestEvents");

    for (; it != beacon.network_request_event_list.end(); ++it)
    {
      network_request_event_t event = (network_request_event_t)(*it);

      appd_iot_json_start_object(json, NULL);

      appd_iot_json_add_string_key_value(json, "url", event.url.c_str());

      if (event.resp_code != 0)
      {
        appd_iot_json_add_integer_key_value(json, "statusCode", event.resp_code);
      }

      if (!event.error.empty())
      {
        appd_iot_json_add_string_key_value(json, "networkError", event.error.c_str());
      }

      if (event.req_content_length > 0)
      {
        appd_iot_json_add_integer_key_value(json, "requestContentLength", event.req_content_length);
      }

      if (event.resp_content_length > 0)
      {
        appd_iot_json_add_integer_key_value(json, "responseContentLength", event.resp_content_length);
      }

      if (event.timestamp_ms != 0)
      {
        appd_iot_json_add_integer_key_value(json, "timestamp", event.timestamp_ms);
      }

      if (event.duration_ms > 0)
      {
        appd_iot_json_add_integer_key_value(json, "duration", event.duration_ms);
      }

      //Response Headers are expected to have {key, value} pairs as strings
      if (!(event.resp_headers.stringmap.empty()))
      {
        appd_iot_json_start_object(json, "responseHeaders");

        for (std::map<std::string, std::string>::iterator resp_header_it = event.resp_headers.stringmap.begin();
             resp_header_it != event.resp_headers.stringmap.end(); ++resp_header_it)
        {
          appd_iot_json_start_array(json, (resp_header_it->first).c_str());
          appd_iot_json_add_string_value(json, (resp_header_it->second).c_str());
          appd_iot_json_end_array(json);
        }

        appd_iot_json_end_object(json);
      }

      appd_iot_serialize_properties_data_to_json(json, &event.data);

      appd_iot_json_end_object(json);
    }

    appd_iot_json_end_array(json);
  } /* End Network Event Processing */


  /* Start Error Event Processing */
  if (beacon.error_event_list.size() != 0)
  {
    std::list<error_event_t>::iterator it = beacon.error_event_list.begin();

    appd_iot_json_start_array(json, "errorEvents");

    for (; it != beacon.error_event_list.end(); ++it)
    {
      error_event_t event = (error_event_t)(*it);

      appd_iot_json_start_object(json, NULL);

      if (!event.name.empty())
      {
        appd_iot_json_add_string_key_value(json, "name", event.name.c_str());
      }

      if (!event.message.empty())
      {
        appd_iot_json_add_string_key_value(json, "message", event.message.c_str());
      }

      if (!event.severity.empty())
      {
        appd_iot_json_add_string_key_value(json, "severity", event.severity.c_str());
      }

      if (event.timestamp_ms != 0)
      {
        appd_iot_json_add_integer_key_value(json, "timestamp", event.timestamp_ms);
      }

      if (event.duration_ms > 0)
      {
        appd_iot_json_add_integer_key_value(json, "duration", event.duration_ms);
      }

      if (!event.stack_trace_list.empty())
      {
        std::list<stack_trace_t>::iterator stack_trace_it = event.stack_trace_list.begin();

        appd_iot_json_add_integer_key_value(json, "errorStackTraceIndex", event.error_stack_trace_index);

        appd_iot_json_start_array(json, "stackTraces");

        //loop over stack traces
        for (; stack_trace_it != event.stack_trace_list.end(); ++stack_trace_it)
        {
          stack_trace_t stack_trace = (stack_trace_t)(*stack_trace_it);

          appd_iot_json_start_object(json, NULL);

          appd_iot_json_add_string_key_value(json, "thread", stack_trace.thread.c_str());
          appd_iot_json_add_string_key_value(json, "runtime", stack_trace.runtime.c_str());

          if (!(stack_trace.stack_frame_list.empty()))
          {
            std::list<stack_frame_t>::iterator stack_frame_it = stack_trace.stack_frame_list.begin();

            appd_iot_json_start_array(json, "stackFrames");

            //loop over stack frames within a single stack trace
            for (; stack_frame_it != stack_trace.stack_frame_list.end(); ++stack_frame_it)
            {
              stack_frame_t stack_frame = (stack_frame_t)(*stack_frame_it);
              appd_iot_json_start_object(json, NULL);

              if (!stack_frame.symbol_name.empty())
              {
                appd_iot_json_add_string_key_value(json, "symbolName", stack_frame.symbol_name.c_str());
                appd_iot_json_add_integer_key_value(json, "symbolOffset", stack_frame.symbol_offset);
              }

              if (!stack_frame.package_name.empty())
              {
                appd_iot_json_add_string_key_value(json, "packageName", stack_frame.package_name.c_str());
              }

              if (!stack_frame.file_name.empty())
              {
                appd_iot_json_add_string_key_value(json, "filePath", stack_frame.file_name.c_str());
              }

              if (stack_frame.lineno > 0)
              {
                appd_iot_json_add_integer_key_value(json, "lineNumber", stack_frame.lineno);
              }

              appd_iot_json_add_integer_key_value(json, "absoluteAddress", stack_frame.absolute_addr);
              appd_iot_json_add_integer_key_value(json, "imageOffset", stack_frame.image_offset);

              appd_iot_json_end_object(json);
            }

            appd_iot_json_end_array(json);
          }

          appd_iot_json_end_object(json);
        }

        appd_iot_json_end_array(json);
      }

      appd_iot_serialize_properties_data_to_json(json, &event.data);

      appd_iot_json_end_object(json);
    }

    appd_iot_json_end_array(json);
  } /* End Error Event Processing */

  appd_iot_json_end_object(json);

  appd_iot_json_end_array(json);

  appd_iot_log(APPD_IOT_LOG_VERBOSE, "JSON BEACON %s", appd_iot_json_pretty_print(json));

  const char* json_str = appd_iot_json_get_string(json);

  std::string ret_str;

  if (json_str != NULL)
  {
    //Assign operator for std::string will make a copy of the string.
    ret_str = json_str;
  }

  /* clearing root json object frees memory for all child json objects */
  appd_iot_json_free(json);

  return ret_str;
}
