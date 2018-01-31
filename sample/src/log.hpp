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

#ifndef _LOG_HPP
#define _LOG_HPP


/**
 * @brief Opens Log file in write mode. If log file doesn't exist, a new one is created.
 * @param logfilename indicates the name of the log file to be opened.
 */
void open_log(const char* logfilename);


/**
 * @brief Prefixes Log Message with timestamp and writes to log file pointed by global_fd. <br>
 * open_log() has to be called before log_write_cb function is triggered.
 * @param logmsg contains the log message without the newline char at the end <br>
 * logmsg is appended with a tag "E/APPDYNAMICS:". First Letter in the tag  <br>
 * indicates log level as given in appd_iot_log_level_t. <br>
 * logmsg not to be freed in the log write callback. It is freed by the caller of log write cb.
 * @param logmsg_len contains the length of log message
 */
void log_write_cb(const char* logmsg, size_t logmsg_len);


/**
 * @brief Closes Log File if opened and sets FD to -1
 */
void close_log();

#endif /* _LOG_HPP */
