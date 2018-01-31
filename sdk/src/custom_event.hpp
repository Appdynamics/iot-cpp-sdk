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

#ifndef _CUSTOM_EVENT_H
#define _CUSTOM_EVENT_H

#include "beacon.hpp"

/**
 * @brief Copies User Defined Event Data to SDK Defined Event Data
 * @param destdata contains event data to be copied to
 * @param srcdata contains event data to be copied from
 * @param srcdata_count contains number of key-value pairs in user defined event data
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_copy_event_data(data_t* destdata, appd_iot_data_t* srcdata,
    int srcdata_count);

/**
 * @brief Clear event data
 * @param data that needs to be cleared
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_clear_event_data(data_t* data);


#endif /* _CUSTOM_EVENT_H */
