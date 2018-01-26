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


#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include "log.hpp"
#include "config.hpp"

//This includes both log header and log message
#define LOG_MAX_SIZE 2048

//Default Log Header. First Character represents log level as given in loglevel_str[]
#define LOG_HEADER "E/APPDYNAMICS: "
#define LOG_HEADER_LEN (sizeof(LOG_HEADER) - 1)

#define ERROR_LOG_MSG "<NULL Log Message>"

static char loglevel_c[APPD_IOT_MAX_LOG_LEVELS] = {'O', 'E', 'W', 'I', 'D', 'V', 'A'};

static void log_write_to_stderr(const char* logmsg, size_t logmsg_len);

static const char* error_code_to_str[APPD_IOT_MAX_ERROR_CODES] =
{
  /*! Success */
  "SUCCESS",
  /*! Invalid Input Passed as Param */
  "INVALID_INPUT",
  /*! Max Limit Reached */
  "MAX_LIMIT",
  /*! Network Request Error (Could be Client Error or Server Error) */
  "NETWORK_ERROR",
  /*! Network Request Rejected by Server */
  "NETWORK_REJECT",
  /*! Host Not Reachable */
  "NETWORK_UNREACHABLE",
  /*! No Network Connectivity */
  "NETWORK_NOT_AVAILABLE",
  /*! Encountered a NULL ptr */
  "NULL_PTR",
  /*! Internal Application Error */
  "INTERNAL_ERROR",
  /*! Not Supported Functionality */
  "NOT_SUPPORTED",
  /*! SDK is not in Enabled State */
  "SDK_NOT_ENABLED",

};

static const char* sdk_state_to_str[APPD_IOT_MAX_SDK_STATES] =
{
  /*! SDK is Uninitialized */
  "SDK_UNINITIALIZED",
  /*! SDK is Enabled */
  "SDK_ENABLED",
  /*!  SDK is Disabled by Controller by manually turning off Instrumentation */
  "SDK_DISABLED_KILL_SWITCH",
  /*! SDK is Disabled by Controller due to License Expiry  */
  "SDK_DISABLED_LICENSE_EXPIRED",
  /*! SDK is Disabled by Controller due to Data Limit Exceeded */
  "SDK_DISABLED_DATA_LIMIT_EXCEEDED"
};

/**
 * @brief Reads log message, appends log header and triggers log write callback function
 * @param log_level indicates log level listed in log_detail_t
 * @param format Printf Format String
 */
void appd_iot_log(appd_iot_log_level_t log_level, const char* format, ...)
{
  //check for log level
  appd_iot_log_level_t configlog_level = appd_iot_get_log_level();

  if (log_level > configlog_level)
  {
    return;
  }

  if (log_level >= APPD_IOT_MAX_LOG_LEVELS)
  {
    log_level = APPD_IOT_LOG_ERROR;
  }

  //check for log write cb
  appd_iot_log_write_cb_t log_write_cb = appd_iot_get_log_write_cb();

  if (log_write_cb == NULL)
  {
    log_write_cb = &log_write_to_stderr;
  }

  //initilize logbuf with log header
  char logbuf[LOG_MAX_SIZE] = LOG_HEADER;
  size_t logmsg_len;

  //update log level in the log header
  logbuf[0] = loglevel_c[log_level];

  va_list args;

  //read log message
  va_start(args, format);
  int nchar = vsnprintf(logbuf + LOG_HEADER_LEN, (LOG_MAX_SIZE - LOG_HEADER_LEN), format, args);
  va_end(args);

  if (nchar < 0)
  {
    snprintf(logbuf + LOG_HEADER_LEN, sizeof(ERROR_LOG_MSG), ERROR_LOG_MSG);
    logmsg_len = LOG_HEADER_LEN + sizeof(ERROR_LOG_MSG) - 1;
  }
  else if (LOG_HEADER_LEN + nchar >= LOG_MAX_SIZE)
  {
    logmsg_len = LOG_MAX_SIZE - 1;
    logbuf[logmsg_len] = '\0';
  }
  else
  {
    logmsg_len = LOG_HEADER_LEN + nchar;
  }

  //trigger callback to write log msg
  log_write_cb(logbuf, logmsg_len);
}

/**
 * @brief Prefixes Log Message with timestamp and writes to stderr. <br>
 * @param logmsg contains the log message without the newline char at the end
 * @param logmsg_len contains the length of log message
 */
static void log_write_to_stderr(const char* logmsg, size_t logmsg_len)
{
  if (logmsg == NULL)
  {
    return;
  }

  int iovcnt;
  struct iovec iov[3];
  char timestamp[32];
  char eol = '\n';

  struct timeval tv;
  gettimeofday(&tv, NULL);

  int64_t time_in_ms = (int64_t)tv.tv_sec * 1000 + (int64_t)tv.tv_usec / 1000;
  size_t timestamp_len = snprintf(timestamp, sizeof(timestamp), "%" PRId64 " ", time_in_ms);

  if (timestamp_len >= sizeof(timestamp))
  {
    timestamp_len = sizeof(timestamp) - 1;
    timestamp[timestamp_len] = '\0';
  }

  iov[0].iov_base = (void*)timestamp;
  iov[0].iov_len = timestamp_len;

  iov[1].iov_base = (void*)logmsg;
  iov[1].iov_len = logmsg_len;

  iov[2].iov_base = (void*)(&eol);
  iov[2].iov_len = sizeof(eol);

  iovcnt = sizeof(iov) / sizeof(struct iovec);

  writev(STDERR_FILENO, iov, iovcnt);
}

/**
 * @brief Convert error code to string
 * @param error_code that is to be converted to string
 * @return string representation of the error code
 */
const char* appd_iot_error_code_to_str(appd_iot_error_code_t error_code)
{
  return error_code_to_str[error_code];
}

/**
 * @brief Convert sdk state to string
 * @param sdk_state that is to be converted to string
 * @return string representation of the sdk state
 */
const char* appd_iot_sdk_state_to_str(appd_iot_sdk_state_t sdk_state)
{
  return sdk_state_to_str[sdk_state];
}
