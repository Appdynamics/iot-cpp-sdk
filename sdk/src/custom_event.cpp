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
#include "custom_event.hpp"
#include "log.hpp"
#include "config.hpp"
#include "utils.hpp"

/**
  * @brief converts custom event data to beacon format and adds to beacon
  * @param custom_event contains event data
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_add_custom_event(appd_iot_custom_event_t custom_event)
{
  appd_iot_error_code_t retcode;
  appd_iot_sdk_state_t sdk_state;
  custom_event_t event;

  if ((sdk_state = appd_iot_get_sdk_state()) != APPD_IOT_SDK_ENABLED)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Add Custom Event Failed. SDK Not in Enabled State:%s",
                 appd_iot_sdk_state_to_str(sdk_state));

    return APPD_IOT_ERR_SDK_NOT_ENABLED;
  }

  if (custom_event.type != NULL)
  {
    event.type = appd_iot_remove_character(custom_event.type, '|');
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Custom Event Type cannot be NULL");
  }

  if (custom_event.summary != NULL)
  {
    event.summary = custom_event.summary;
  }
  else
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Custom Event Summary is NULL");
  }

  event.timestamp_ms = custom_event.timestamp_ms;
  event.duration_ms = custom_event.duration_ms;
  retcode = APPD_IOT_SUCCESS;

  if (custom_event.data_count > 0)
  {
    retcode = appd_iot_copy_event_data(&event.data, custom_event.data,
                                       custom_event.data_count);

    if (retcode != APPD_IOT_SUCCESS)
    {
      appd_iot_log(APPD_IOT_LOG_WARN, "Failed to parse custom event data, error:%s",
                   appd_iot_error_code_to_str(retcode));

      appd_iot_clear_event_data(&event.data);
    }
  }

  appd_iot_log(APPD_IOT_LOG_INFO, "Adding Custom Event with Type:%s", event.type.c_str());

  retcode = appd_iot_add_custom_event_to_beacon(event);

  return retcode;
}

/**
 * @brief Clear event data
 * @param data that needs to be cleared
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_clear_event_data(data_t* data)
{
  data->stringmap.clear();
  data->integermap.clear();
  data->boolmap.clear();
  data->datetimemap.clear();
  data->doublemap.clear();

  return APPD_IOT_SUCCESS;
}

/**
 * @brief Copies User Defined Event Data to SDK Defined Event Data
 * @param destdata contains event data to be copied to
 * @param srcdata contains event data to be copied from
 * @param srcdata_count contains number of key-value pairs in user defined event data
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_copy_event_data
(data_t* destdata, appd_iot_data_t* srcdata, int srcdata_count)
{
  if (destdata == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Destination Data Pointer in NULL");
    return APPD_IOT_ERR_INTERNAL;
  }

  if (srcdata == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Source Data Pointer is NULL");
    return APPD_IOT_ERR_NULL_PTR;
  }

  for (int i = 0; i < srcdata_count; i++)
  {
    if ((srcdata + i) == NULL)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Event Data at Index:%d is NULL", i);
      return APPD_IOT_ERR_NULL_PTR;
    }

    if (srcdata[i].key == NULL)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Event <Key> at Index:%d is NULL", i);
      return APPD_IOT_ERR_NULL_PTR;
    }

    std::string key = appd_iot_remove_character(srcdata[i].key, '|');

    switch (srcdata[i].value_type)
    {
      case APPD_IOT_INTEGER:
      {
        destdata->integermap[key] = srcdata[i].intval;
        break;
      }

      case APPD_IOT_DOUBLE:
      {
        destdata->doublemap[key] = srcdata[i].doubleval;
        break;
      }

      case APPD_IOT_BOOLEAN:
      {
        destdata->boolmap[key] = srcdata[i].boolval;
        break;
      }

      case APPD_IOT_STRING:
      {
        if (srcdata[i].strval != NULL)
        {
          destdata->stringmap[key] = srcdata[i].strval;
        }
        else
        {
          appd_iot_log(APPD_IOT_LOG_WARN, "String Value is NULL for key %s", srcdata[i].key);
        }

        break;
      }

      case APPD_IOT_DATETIME:
      {
        destdata->datetimemap[key] = srcdata[i].datetimeval;
        break;
      }

      default:
      {
        appd_iot_log(APPD_IOT_LOG_ERROR, "Skip Adding event field:%s due to invalid data type:%d",
                     srcdata[i].key, srcdata[i].value_type);
        break;
      }
    }

    appd_iot_log(APPD_IOT_LOG_INFO, "Added Key :%s with value type:%d", key.c_str(), srcdata[i].value_type);
  }

  return APPD_IOT_SUCCESS;
}


/**
  * @brief send all events to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_send_all_events(void)
{
  appd_iot_sdk_state_t sdk_state;

  if ((sdk_state = appd_iot_get_sdk_state()) != APPD_IOT_SDK_ENABLED)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Send All Events Failed. SDK Not in Enabled State:%s",
                 appd_iot_sdk_state_to_str(sdk_state));

    return APPD_IOT_ERR_SDK_NOT_ENABLED;
  }

  return appd_iot_send_all_beacons();
}


/**
  * @brief Clear all events to collector
  * @return appd_iot_error_code_t indicating function execution status
  */
appd_iot_error_code_t appd_iot_clear_all_events(void)
{
  appd_iot_sdk_state_t sdk_state;

  if ((sdk_state = appd_iot_get_sdk_state()) != APPD_IOT_SDK_ENABLED)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Clear All Events Failed. SDK Not in Enabled State:%s",
                 appd_iot_sdk_state_to_str(sdk_state));

    return APPD_IOT_ERR_SDK_NOT_ENABLED;
  }

  return appd_iot_clear_all_beacons();
}
