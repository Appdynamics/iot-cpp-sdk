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

//To enable format macros like PRId64
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "log_mock_interface.hpp"

static bool global_log_write_cb_log_msg_not_null;
static bool global_log_write_cb_triggered;

/**
 * @brief Check if log write callback is triggerd and <br>
 * if the log message received is not null
 */
bool appd_iot_is_log_write_cb_success()
{
  return (global_log_write_cb_triggered && global_log_write_cb_log_msg_not_null);
}

/**
 * @brief Clear flags tracking if log write callback is triggered and if log msg is null
 */
void appd_iot_clear_log_write_cb_flags()
{
  global_log_write_cb_triggered = false;
  global_log_write_cb_log_msg_not_null = true;
}

/**
 * @brief Writes the log message to stderr if the log message is not null
 */
void appd_iot_log_write_cb(const char* logmsg, size_t logmsg_len)
{
  global_log_write_cb_triggered = true;

  if ((logmsg == NULL) || (strlen(logmsg) != logmsg_len))
  {
    global_log_write_cb_log_msg_not_null = false;
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
