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

#include "utils.hpp"
#include "log.hpp"

/**
 * @brief Removes a given character from the input string
 * @param input contains input string
 * @param c contains the character that needs to be removed from input string.
 * @return new string formed by removing character c from input string.
 * If the input string doesn't contain character c, output string will be same as input string.
 */
std::string appd_iot_remove_character(const char* input, char c)
{
  std::string output;
  bool flag = false;

  if (input == NULL)
  {
    return output;
  }

  for (size_t i = 0; input[i] != '\0'; i++)
  {
    if (input[i] == c)
    {
      flag = true;
      continue;
    }

    output.append(&(input[i]), 1);
  }

  if (flag)
  {
    appd_iot_log(APPD_IOT_LOG_WARN, "Removed invalid '|' pipe char from input string: %s", input);
  }

  return output;
}
