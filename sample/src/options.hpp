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

#ifndef options_hpp
#define options_hpp

#include <stdio.h>

enum event_type
{
  CUSTOM_EVENT = 0x1,
  NETWORK_EVENT = 0x2,
  ERROR_EVENT = 0x4,
  ALL_EVENTS = 0x7
};

typedef struct
{
  const char* url;
  const char* type;
  const char* data;
} http_req_options_t;

/**
 * @brief Read Command Line Arguments
 * @return status of the function execution
 */
bool read_options(int argc, char* argv[]);

/**
 * @brief Frees up any memory allocated for all the options read
 */
void free_options(void);

/**
 * @brief Get AppKey provided as input to sample app execution
 * @return Appkey
 */
const char* get_app_key(void);

/**
 * @brief Get Collector URL provided as one of the options to sample app execution
 * @return Collector URL
 */
const char* get_collector_url(void);

/**
 * @brief Get Logfile name provided as one of the options to sample app execution <br>
 * If no options given for logfile, return default log file given by macro DEFAULT_LOG_FILE
 * @return Logfile Name
 */
const char* get_log_file(void);

/**
 * @brief Get Loglevel provided as one of the options to sample app execution <br>
 * If no options given for Loglevel, return default log level as APPD_IOT_LOG_ALL
 * @return Loglevel
 */
appd_iot_log_level_t get_log_level(void);


/**
 * @brief Get eventtype provided as one of the options to sample app execution <br>
 * If no options given for eventtype, return default event type as <br>
 * All(0x7) events - custom, network and error events
 * @return event type
 */
event_type get_event_type(void);

/**
 * @brief Get Timer Value for the periodicity with which to check with AppDynamics Collector if SDK can be
 * enabled
 * @return Timer Value in Seconds
 */
int get_timer_value_in_sec(void);

/**
 * @brief Get Number of Times to check periodically with AppDynamics Collector if SDK can be enabled
 * @return Number of Retries
 */
int get_num_retries(void);

/**
 * @brief Get Http Request Options provided in sample app execution <br>
 * Http Request Options include URL, Request Type and Data.
 * @return Http Request Options
 */
http_req_options_t* get_http_req_options();

#endif /* options_hpp */
