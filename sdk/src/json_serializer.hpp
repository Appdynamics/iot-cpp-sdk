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


#ifndef _JSON_SERIALIZER_HPP
#define _JSON_SERIALIZER_HPP

#include <appd_iot_interface.h>

/**
 * @brief Enums to keep track of last JSON Operation completed
 */
typedef enum
{
  DEINIT,
  INIT,
  START_ARRAY,
  START_OBJECT,
  ADD_DATA,
  END_OBJECT,
  END_ARRAY
} json_ops_t;

/**
 * @brief JSON struct which holds json string in a buffer.
 */
typedef struct
{
  size_t len;
  size_t max_len;
  char* buf;
  char* printbuf;
  json_ops_t last_op;
} json_t;

/**
 * @brief Creates, Initializes and returns a new json struct
 * @return json_t pointer to the newly created json object
 */
json_t* appd_iot_json_init(void);

/**
 * @brief starts json array by adding char '[' to json buf.
 * If a name is given it will be "name":[
 * @param json struct which contains the json buf
 * @param array_name has the string for the key to json array
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_start_array(json_t* json, const char* array_name);

/**
 * @brief starts json object by adding char '{' to json buf.
 * If a name is given it will be "name":{
 * @param json struct which contains the json buf
 * @param object_name has the string for the key to json object
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_start_object(json_t* json, const char* object_name);

/**
 * @brief ends json object by adding char '}' to json buf.
 * @param json struct which contains the json buf
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_end_object(json_t* json);

/**
 * @brief ends json object by adding char ']' to json buf.
 * @param json struct which contains the json buf
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_end_array(json_t* json);

/**
 * @brief adds key:value pair to json object with value as string
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param strval contains string representing value
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_string_key_value
(json_t* json, const char* key, const char* strval);

/**
 * @brief adds key:value pair to json object with value as 64 bit integer
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param intval contains integer representing value
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_integer_key_value(json_t* json, const char* key, int64_t intval);

/**
 * @brief adds key:value pair to json object with value as double
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param doubleval contains double representing value
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_double_key_value(json_t* json, const char* key, double doubleval);

/**
 * @brief adds key:value pair to json object with value as boolean
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param boolval contains boolean representing value
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_boolean_key_value(json_t* json, const char* key, bool boolval);

/**
 * @brief adds value to json object. This function is typically used to add string values to JSON arrays
 * @param json struct which contains the json buf
 * @param value contains string to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_string_value(json_t* json, const char* value);

/**
 * @brief adds value to json object. This function is typically used to add integer values to JSON arrays
 * @param json struct which contains the json buf
 * @param intval contains integer to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_integer_value(json_t* json, int64_t intval);

/**
 * @brief adds value to json object. This function is typically used to add double values to JSON arrays
 * @param json struct which contains the json buf
 * @param doubleval contains integer to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_double_value(json_t* json, double doubleval);

/**
 * @brief adds value to json object. This function is typically used to add boolean values to JSON arrays
 * @param json struct which contains the json buf
 * @param boolval contains boolean to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_boolean_value(json_t* json, bool boolval);

/**
 * @brief returns the json string constructed so far.
 * @param json struct which contains the json buf
 * @return char pointer containing the json string.
 */
const char* appd_iot_json_get_string(json_t* json);

/**
 * @brief format and returns json string with line breaks, indentiation at start/end of objects/arrays
 * @param json struct which contains the json buf
 * @return char pointer containing the json string.
 */
const char* appd_iot_json_pretty_print(json_t* json);

/**
 * @brief frees json structure
 * @param json struct which contains the json buf
 */
void appd_iot_json_free(json_t* json);

#endif // _JSON_SERIALIZER_HPP

