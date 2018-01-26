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

#ifndef custom_event_hpp
#define custom_event_hpp

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <appd_iot_interface.h>

/**
 * @brief Fills a custom event structure and <br>
 * calls IoT CPP SDK API to add and send the event
 */
void send_custom_event(void);

#endif /* custom_event_hpp */
