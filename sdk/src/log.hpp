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

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <appd_iot_interface.h>

/**
 * @brief Reads log message, appends log header and triggers log write callback function
 * @param log_level indicates log level listed in log_detail_t
 * @param format Printf Format String
 */
void appd_iot_log (appd_iot_log_level_t log_level,
                   const char* format, ...) __attribute__((format(printf, 2, 3)));

#endif // _LOG_H
