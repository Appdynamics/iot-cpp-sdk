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

#include <getopt.h>
#include <stdlib.h>
#include <appd_iot_interface.h>
#include "options.hpp"

#define DEFAULT_LOG_FILE "appd_iot_sdk.log"
#define DEFAULT_LOG_LEVEL APPD_IOT_LOG_ALL
#define DEFAULT_EVENT_TYPE ALL_EVENTS
#define DEFAULT_HTTP_REQ_TYPE "GET"

static const char* logfile;
static const char* collector_url;
static const char* appkey;
static appd_iot_log_level_t loglevel = DEFAULT_LOG_LEVEL;
static event_type event = DEFAULT_EVENT_TYPE;
static int timer_value_sec = 15;
static int num_retries = 5;

static http_req_options_t http_req_options;

/**
 * @brief Print Usage Information of the Simple App
 */
void show_help(const char* program)
{
  if (program == NULL)
  {
    return;
  }

  fprintf(stdout, "\n");
  fprintf(stdout, "Simple Program to Fill and Send Sample Data for Custom, Network, and Error Events\n\n");

  fprintf(stdout, "USAGE: %s <appkey> [options]\n\n", program);

  fprintf(stdout, "options:\n");

  fprintf(stdout, "-c, --collectorurl <url>   Collector URL to which sdk will send events to.\n");
  fprintf(stdout, "                           Default collector set to APPD Production Collector \n");

  fprintf(stdout, "-f, --logfile <file>       Name of log file to which sdk log messages are written to.\n");
  fprintf(stdout, "                           Default Log file name set to \"appd_iot_sdk.log\"\n");

  fprintf(stdout, "-l, --loglevel <level>     Log level set to an integer value mapping to: \n");
  fprintf(stdout, "                           1-error, 2-warn, 3-info, 4-debug, 5-verbose, 6-all.\n");
  fprintf(stdout, "                           Default log level is set to 6 \n");

  fprintf(stdout, "-s, --sendevent <event>    Sample event to be sent. Provide an integer value mapping to\n");
  fprintf(stdout, "                           one of the following values:\n");
  fprintf(stdout, "                           1-custom, 2-network, 4-error, 7-all. Default is set to 7.\n");

  fprintf(stdout, "-u, --url <url>            URL to trigger network request and capture network event.\n");

  fprintf(stdout, "-x, --request <command>    Specify request type for the url. Default is set to GET\n");

  fprintf(stdout, "-d, --data <data>          Data in JSON format that is to be sent in a POST request.\n");

  fprintf(stdout, "-t, --timer <value>        Timer value in seconds to periodically check if sdk can be\n");
  fprintf(stdout, "                           enabled. Default timer value set to 15 seconds.\n");

  fprintf(stdout, "-r, --retries <value>      Number of times to check if sdk can be enabled with a \n");
  fprintf(stdout, "                           perodicity given by timer value. Default Retries set to 5\n");

  fprintf(stdout, "-h, --help                 Display available options\n");
}

/**
 * @brief Read Command Line Arguments
 * @return status of the function execution
 */
bool read_options(int argc, char* argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Expected argument <appkey> is missing \n");
    show_help(argv[0]);
    return false;
  }

  int long_index = 0;
  int opt = 0;
  long l = 0;
  long s = 0;

  struct option long_options[] =
  {
    {"collectorurl", required_argument, NULL,  'c' },
    {"logfile", required_argument, NULL,  'f' },
    {"loglevel", required_argument, NULL,  'l' },
    {"sendevent", required_argument, NULL, 's'},
    {"url", required_argument, NULL, 'u'},
    {"request", required_argument, NULL, 'x'},
    {"data", required_argument, NULL, 'd'},
    {"timer", required_argument, NULL, 't'},
    {"retries", required_argument, NULL, 'r'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
  };

  //this call modifies the argv and pushes all non option fields to the end
  while ((opt = getopt_long(argc, argv, "c:f:l:s:u:x:d:t:r:h",
                            long_options, &long_index )) != -1)
  {
    switch (opt)
    {
      case 'c':
        collector_url = strdup(optarg);
        break;

      case 'h':
        show_help(argv[0]);
        return false;
        break;

      case 'f':
        logfile = strdup(optarg);
        break;

      case 'l':
        l = strtol(optarg, NULL, 10);
        break;

      case 's':
        s = strtol(optarg, NULL, 10);
        break;

      case 'u':
        http_req_options.url = strdup(optarg);
        break;

      case 'x':
        http_req_options.type = strdup(optarg);
        break;

      case 'd':
        http_req_options.data = strdup(optarg);
        break;

      case 't':
        timer_value_sec = (int)strtol(optarg, NULL, 10);
        break;

      case 'r':
        num_retries = (int)strtol(optarg, NULL, 10);
        break;

      default:
        show_help(argv[0]);
        return false;
        break;
    }
  }

  //set default parameters
  if (logfile == NULL)
  {
    logfile =  strdup(DEFAULT_LOG_FILE);
  }

  if (http_req_options.url != NULL)
  {
    if (http_req_options.type == NULL)
    {
      http_req_options.type = strdup(DEFAULT_HTTP_REQ_TYPE);
    }
  }

  //check if log level is valid
  if (l > APPD_IOT_LOG_OFF && l <= APPD_IOT_LOG_ALL)
  {
    loglevel = (appd_iot_log_level_t)l;
  }

  //check if event type is one of custom, network or error event
  if ((s & ALL_EVENTS) && (s <= ALL_EVENTS))
  {
    event = (event_type)s;
  }

  //optind points to the index where non-option fields start
  //check for how many non-option fields present
  if (optind >= argc)
  {
    fprintf(stderr, "Expected argument <appkey> is missing \n");
    show_help(argv[0]);
    return false;
  }

  //expecting only one non-option argument, discard any other non-option arguments
  appkey = strdup(argv[optind]);

  return true;
}

/**
 * @brief Frees up any memory allocated for all the options read
 */
void free_options()
{
  if (collector_url)
  {
    free((void*)collector_url);
  }

  if (logfile)
  {
    free((void*)logfile);
  }

  if (appkey)
  {
    free((void*)appkey);
  }

  if (http_req_options.url)
  {
    free((void*)http_req_options.url);
  }

  if (http_req_options.type)
  {
    free((void*)http_req_options.type);
  }

  if (http_req_options.data)
  {
    free((void*)http_req_options.data);
  }
}

/**
 * @brief Get AppKey provided as input to sample app execution
 * @return Appkey
 */
const char* get_app_key()
{
  return appkey;
}

/**
* @brief Get Collector URL provided as one of the options to sample app execution
* @return Collector URL
*/
const char* get_collector_url()
{
  return collector_url;
}

/**
 * @brief Get Logfile name provided as one of the options to sample app execution <br>
 * If no options given for logfile, return default log file given by macro DEFAULT_LOG_FILE
 * @return Logfile Name
 */
const char* get_log_file()
{
  return logfile;
}

/**
 * @brief Get Loglevel provided as one of the options to sample app execution <br>
 * If no options given for Loglevel, return default log level as APPD_IOT_LOG_ALL
 * @return Loglevel
 */
appd_iot_log_level_t get_log_level()
{
  return loglevel;
}

/**
 * @brief Get eventtype provided as one of the options to sample app execution <br>
 * If no options given for eventtype, return default event type as <br>
 * All(0x7) events - custom, network and error events
 * @return event type
 */
event_type get_event_type(void)
{
  return event;
}

/**
 * @brief Get Timer Value for the periodicity with which to check with AppDynamics Collector if SDK can be
 * enabled
 * @return Timer Value in Seconds
 */
int get_timer_value_in_sec(void)
{
  return timer_value_sec;
}

/**
 * @brief Get Number of Times to check periodically with AppDynamics Collector if SDK can be enabled
 * @return Number of Retries
 */
int get_num_retries(void)
{
  return num_retries;
}

/**
 * @brief Get Http Request Options provided in sample app execution <br>
 * Http Request Options include URL, Request Type and Data.
 * @return Http Request Options
 */
http_req_options_t* get_http_req_options()
{
  return &http_req_options;
}


