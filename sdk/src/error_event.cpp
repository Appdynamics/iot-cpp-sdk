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

#include "beacon.hpp"
#include "log.hpp"
#include "config.hpp"
#include "custom_event.hpp"

static const char* severity_str[APPD_IOT_ERR_MAX_SEVERITY_LEVELS] = {"alert", "critical", "fatal"};

static appd_iot_error_code_t appd_iot_copy_stack_trace
(std::list<stack_trace_t>* dest_stack_trace_list, appd_iot_stack_trace_t* src_stack_trace,
 int src_stack_trace_count);

/**
  * @brief converts error event data to beacon format and adds to beacon
  * @param error_event contains event data
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_error_event(appd_iot_error_event_t error_event)
{
  appd_iot_error_code_t retcode;
  appd_iot_sdk_state_t sdk_state;
  error_event_t event;

  if ((sdk_state = appd_iot_get_sdk_state()) != APPD_IOT_SDK_ENABLED)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Add Error Event Failed. SDK Not in Enabled State:%s",
                 appd_iot_sdk_state_to_str(sdk_state));

    return APPD_IOT_ERR_SDK_NOT_ENABLED;
  }

  retcode = APPD_IOT_SUCCESS;
  event.timestamp_ms = error_event.timestamp_ms;
  event.duration_ms = error_event.duration_ms;

  if (error_event.name != NULL)
  {
    event.name = error_event.name;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Error Event Name cannot be NULL");
  }

  if (error_event.message != NULL)
  {
    event.message = error_event.message;
  }

  if (error_event.severity < APPD_IOT_ERR_MAX_SEVERITY_LEVELS)
  {
    event.severity = severity_str[error_event.severity];
  }
  else
  {
    event.severity = severity_str[APPD_IOT_ERR_SEVERITY_CRITICAL];
  }

  if (error_event.stack_trace_count > 0)
  {
    if (error_event.error_stack_trace_index >= error_event.stack_trace_count)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Invalid error stack trace index, setting to index 0");

      event.error_stack_trace_index = 0;
    }
    else
    {
      event.error_stack_trace_index = error_event.error_stack_trace_index;
    }

    retcode = appd_iot_copy_stack_trace(&event.stack_trace_list,
                                        error_event.stack_trace,
                                        error_event.stack_trace_count);

    if (retcode != APPD_IOT_SUCCESS)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to parse stack traces, error:%s",
                   appd_iot_error_code_to_str(retcode));

      event.stack_trace_list.clear();
    }
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_INFO, "No stack traces present");
  }

  if (error_event.data_count > 0)
  {
    retcode = appd_iot_copy_event_data(&event.data, error_event.data,
                                       error_event.data_count);

    if (retcode != APPD_IOT_SUCCESS)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to parse error event data, error:%s",
                   appd_iot_error_code_to_str(retcode));

      appd_iot_clear_event_data(&event.data);
    }
  }

  appd_iot_log(APPD_IOT_LOG_INFO, "Adding Error Event with name:%s", error_event.name);

  retcode = appd_iot_add_error_event_to_beacon(event);

  return retcode;
}

/**
 * @brief Copies User Defined Stack Trace to SDK Defined Stack Trace
 * @param dest_stack_trace_list contains stack trace list to be copied to
 * @param src_stack_trace contains stack trace list to be copied from
 * @param src_stack_trace_count contains number of stack traces
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t appd_iot_copy_stack_trace
(std::list<stack_trace_t>* dest_stack_trace_list, appd_iot_stack_trace_t* src_stack_trace,
 int src_stack_trace_count)
{

  if (dest_stack_trace_list == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "List to copy stack trace is NULL");
    return APPD_IOT_ERR_INTERNAL;
  }

  if (src_stack_trace == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Error Event Stack Trace is NULL");
    return APPD_IOT_ERR_NULL_PTR;
  }

  for (int i = 0; i < src_stack_trace_count; i++)
  {
    stack_trace_t dest_stack_trace;

    dest_stack_trace.runtime = "native";

    //dest_stack_trace has default values as empty strings
    if (src_stack_trace->thread != NULL)
    {
      dest_stack_trace.thread = src_stack_trace->thread;
    }

    for (int j = 0; j < src_stack_trace->stack_frame_count; j++)
    {
      stack_frame_t dest_stack_frame;

      if ((src_stack_trace->stack_frame + j) == NULL)
      {
        appd_iot_log(APPD_IOT_LOG_ERROR, "NULL stack frame found inside stack trace");
        return APPD_IOT_ERR_NULL_PTR;
      }

      appd_iot_stack_frame_t src_stack_frame = src_stack_trace->stack_frame[j];

      //dest_stack_frame has default values as empty strings
      if (src_stack_frame.symbol_name != NULL)
      {
        dest_stack_frame.symbol_name = src_stack_frame.symbol_name;
      }

      if (src_stack_frame.package_name != NULL)
      {
        dest_stack_frame.package_name = src_stack_frame.package_name;
      }

      if (src_stack_frame.file_name != NULL)
      {
        dest_stack_frame.file_name = src_stack_frame.file_name;
      }

      dest_stack_frame.lineno = src_stack_frame.lineno;
      dest_stack_frame.absolute_addr = src_stack_frame.absolute_addr;
      dest_stack_frame.image_offset = src_stack_frame.image_offset;
      dest_stack_frame.symbol_offset = src_stack_frame.symbol_offset;

      dest_stack_trace.stack_frame_list.push_back(dest_stack_frame);
    }

    dest_stack_trace_list->push_back(dest_stack_trace);
  }

  return APPD_IOT_SUCCESS;
}
