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

#include "error_event.hpp"

/**
 * @brief Fills alert error event structure and <br>
 * calls IoT CPP SDK API to add and send the event
 */
void send_error_event_alert(void)
{

  appd_iot_error_event_t error_event;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&error_event, sizeof(error_event));

  error_event.name = "Warning Light";
  error_event.message = "Oil Change Reminder";
  error_event.severity = APPD_IOT_ERR_SEVERITY_ALERT;
  error_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  error_event.duration_ms = 0;

  error_event.data_count = 1;
  error_event.data = (appd_iot_data_t*)calloc(error_event.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_integer(&error_event.data[0], "Mileage", 27300);

  retcode = appd_iot_add_error_event(error_event);
  fprintf(stdout, "Add Alert Error Event Status :%s\n", appd_iot_error_code_to_str(retcode));

  free(error_event.data);

  if (retcode == APPD_IOT_SUCCESS)
  {
    retcode = appd_iot_send_all_events();
    fprintf(stdout, "Send Alert Error Event Status :%s\n\n", appd_iot_error_code_to_str(retcode));
  }
}


/**
 * @brief Fills a critical event structure and <br>
 * calls IoT CPP SDK API to add and send the event
 */
void send_error_event_critical(void)
{

  appd_iot_error_event_t error_event;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&error_event, sizeof(error_event));

  error_event.name = "Bluetooth Connection Error";
  error_event.message = "connection dropped during voice call due to bluetooth exception";
  error_event.severity = APPD_IOT_ERR_SEVERITY_CRITICAL;
  error_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  error_event.duration_ms = 0;

  error_event.data_count = 3;
  error_event.data = (appd_iot_data_t*)calloc(error_event.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&error_event.data[0], "UUID", "00001101-0000-1000-8000-00805f9b34fb");
  appd_iot_data_set_string(&error_event.data[1], "Bluetooth Version", "3.0");
  appd_iot_data_set_integer(&error_event.data[2], "Error Code", 43);

  retcode = appd_iot_add_error_event(error_event);
  fprintf(stdout, "Add Critical Error Event Status :%s\n", appd_iot_error_code_to_str(retcode));

  free(error_event.data);

  if (retcode == APPD_IOT_SUCCESS)
  {
    retcode = appd_iot_send_all_events();
    fprintf(stdout, "Send Critical Error Event Status :%s\n\n", appd_iot_error_code_to_str(retcode));
  }
}

/**
 * @brief Fills a fatal event structure and <br>
 * calls IoT CPP SDK API to add and send the event. <br>
 * This event shows how a stack trace could be populated
 */
void send_error_event_fatal(void)
{
  appd_iot_error_event_t error_event;
  appd_iot_error_code_t retcode;

  appd_iot_init_to_zero(&error_event, sizeof(error_event));

  error_event.name = "I/O Exception";
  error_event.message = "error while writing data to file";
  error_event.severity = APPD_IOT_ERR_SEVERITY_FATAL;
  error_event.timestamp_ms = ((int64_t)time(NULL) * 1000);
  error_event.duration_ms = 0;
  error_event.stack_trace_count = 1;
  error_event.error_stack_trace_index = 0;

  appd_iot_stack_trace_t* stack_trace = (appd_iot_stack_trace_t*)calloc(error_event.stack_trace_count,
                                        sizeof(appd_iot_stack_trace_t));
  stack_trace->stack_frame_count = 4;
  stack_trace->thread = "main";
  appd_iot_stack_frame_t* stack_frame = (appd_iot_stack_frame_t*)calloc(stack_trace->stack_frame_count,
                                        sizeof(appd_iot_stack_frame_t));
  stack_trace->stack_frame = stack_frame;

  stack_frame[0].symbol_name = "_libc_start_main";
  stack_frame[0].package_name = "/system/lib/libc.so";
  stack_frame[0].absolute_addr = 0x7f8bd984876c;

  stack_frame[1].symbol_name = "main";
  stack_frame[1].package_name = "/home/native-app/mediaplayer/build/mediaplayer_main.so";
  stack_frame[1].absolute_addr = 0x7f8bd984876c;
  stack_frame[1].image_offset = 18861;
  stack_frame[1].symbol_offset = 10;
  stack_frame[1].file_name = "main.c";
  stack_frame[1].lineno = 71;

  stack_frame[2].symbol_name = "write_data";
  stack_frame[2].package_name = "/home/native-app/mediaplayer/build/mediaplayer_main.so";
  stack_frame[2].absolute_addr = 0x7f8bda3f915b;
  stack_frame[2].image_offset = 116437;
  stack_frame[2].symbol_offset = 12;
  stack_frame[2].file_name = "writedata.c";
  stack_frame[2].lineno = 271;

  stack_frame[3].symbol_name = "write_to_file";
  stack_frame[3].package_name = "/home/native-app/mediaplayer/build/mediaplayer_main.so";
  stack_frame[3].absolute_addr = 0x7f8bda9f69d1;
  stack_frame[3].image_offset = 287531;
  stack_frame[3].symbol_offset = 34;
  stack_frame[3].file_name = "writedata.c";
  stack_frame[3].lineno = 524;

  error_event.stack_trace = stack_trace;

  error_event.data_count = 4;
  error_event.data = (appd_iot_data_t*)calloc(error_event.data_count, sizeof(appd_iot_data_t));

  appd_iot_data_set_string(&error_event.data[0], "filename", "buffer-126543.mp3");
  appd_iot_data_set_integer(&error_event.data[1], "filesize", 120000000);
  appd_iot_data_set_integer(&error_event.data[2], "signal number", 6);
  appd_iot_data_set_string(&error_event.data[3], "mediaplayer_version", "1.1");

  retcode = appd_iot_add_error_event(error_event);
  fprintf(stdout, "Add Fatal Error Event Status :%s\n", appd_iot_error_code_to_str(retcode));

  free(error_event.stack_trace->stack_frame);
  free(error_event.stack_trace);
  free(error_event.data);

  if (retcode == APPD_IOT_SUCCESS)
  {
    retcode = appd_iot_send_all_events();
    fprintf(stdout, "Send Fatal Error Event Status :%s\n\n", appd_iot_error_code_to_str(retcode));
  }
}
