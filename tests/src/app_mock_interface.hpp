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

/* Mock Interface for Application using IoT SDK */

#ifndef app_mock_interface_h
#define app_mock_interface_h

#include <appd_iot_interface.h>

/**
 * @brief Get sdk state change cb triggered status
 */
bool appd_iot_mock_get_sdk_state_change_cb_triggered(void);

/**
 * @brief Set sdk state change  cb triggered status
 */
void appd_iot_mock_set_sdk_state_change_cb_triggered(bool status);


/**
 * @brief Get current sdk state
 */
appd_iot_sdk_state_t appd_iot_mock_get_sdk_state(void);

/**
 * @brief Callback function triggered by AppDynamics IoT SDK whenever SDK state changes
 */
void appd_iot_mock_sdk_state_change_cb(appd_iot_sdk_state_t sdk_state);

#endif /* app_mock_interface_h */
