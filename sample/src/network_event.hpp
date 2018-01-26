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

#ifndef network_event_hpp
#define network_event_hpp

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <appd_iot_interface.h>

/**
 * @brief Fills a network event structure and <br>
 * calls IoT CPP SDK API to add and send the event
 */
void send_network_event(void);

/* @brief Trigger HTTP Request to server URL and capture network event <br>
 * Send the captured network event to collector.
 * @param url to which a HTTP Request is triggered
 * @param reqtype specifies the type of HTTP Request (GET or POST)
 * @param reqdata specifies any data that needs to be sent as part of HTTP Request
 */
void capture_and_send_network_event(const char* url, const char* type, const char* data);

#endif /* network_event_hpp */
