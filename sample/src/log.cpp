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

#include <stdio.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdint.h>
#include <inttypes.h>
#include "log.hpp"

static int global_fd = -1;

/**
 * @brief Opens Log file in write mode. If log file doesn't exist, a new one is created.
 * @param logfilename indicates the name of the log file to be opened.
 */
void open_log(const char* logfilename)
{
  if (logfilename == NULL)
  {
    fprintf(stderr, "logfile name is NULL\n");
    return;
  }

  if (global_fd != -1)
  {
    fprintf(stderr, "log file already opened [fd:%d]\n", global_fd);
    return;
  }

  //create and open file in write only, append mode.
  global_fd = open(logfilename, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);

  if (global_fd == -1)
  {
    fprintf(stderr, "open() log file:%s failed with error [%s]\n", logfilename, strerror(errno));
  }

  return;
}


/**
 * @brief Prefixes Log Message with timestamp and writes to log file pointed by global_fd. <br>
 * open_log() has to be called before log_write_cb function is triggered.
 * @param logmsg contains the log message without the newline char at the end <br>
 * logmsg is appended with a tag "E/APPDYNAMICS:". First Letter in the tag  <br>
 * indicates log level as given in appd_iot_log_level_t. <br>
 * logmsg not to be freed in the log write callback. It is freed by the caller of log write cb.
 * @param logmsg_len contains the length of log message
 */
void log_write_cb(const char* logmsg, size_t logmsg_len)
{
  if (global_fd <= 0)
  {
    return;
  }

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

  /* Adjust timestamp_len if number of bytes available to write exceeds size of timestamp buffer */
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

  writev(global_fd, iov, iovcnt);
}


/**
 * @brief Closes Log File if opened and sets FD to -1
 */
void close_log()
{
  if (global_fd != -1)
  {
    close(global_fd);
    global_fd = -1;
  }

  return;
}
