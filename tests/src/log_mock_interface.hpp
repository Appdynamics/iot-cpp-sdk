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

#ifndef log_mock_interface_hpp
#define log_mock_interface_hpp

/**
 * @brief Check if log write callback is triggerd and <br>
 * if the log message received is not null
 */
bool appd_iot_is_log_write_cb_success();


/**
 * @brief Clear flags tracking if log write callback is triggered and if log msg is null
 */
void appd_iot_clear_log_write_cb_flags();

/**
 * @brief Writes the log message to stderr if the log message is not null
 */
void appd_iot_log_write_cb(const char* logmsg, size_t logmsg_size);

#endif /* log_mock_interface_hpp */
