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

/* Mock Interface for Application using IoT SDK */

#include "app_mock_interface.hpp"
#include <stdio.h>

static appd_iot_sdk_state_t global_sdk_state;
static bool sdk_state_change_cb_triggered = false;

/**
 * @brief Callback function triggered by AppDynamics IoT SDK whenever SDK state changes
 */
void appd_iot_mock_sdk_state_change_cb(appd_iot_sdk_state_t sdk_state)
{
  global_sdk_state = sdk_state;
  appd_iot_mock_set_sdk_state_change_cb_triggered(true);
}

/**
 * @brief Get sdk state change cb triggered status
 */
bool appd_iot_mock_get_sdk_state_change_cb_triggered(void)
{
  return sdk_state_change_cb_triggered;
}

/**
 * @brief Set sdk state change  cb triggered status
 */
void appd_iot_mock_set_sdk_state_change_cb_triggered(bool status)
{
  sdk_state_change_cb_triggered = status;
}

/**
 * @brief Get current sdk state
 */
appd_iot_sdk_state_t appd_iot_mock_get_sdk_state(void)
{
  return global_sdk_state;
}

