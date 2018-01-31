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

//To enable format macros like PRId64
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <stdlib.h>
#include <string>
#include <inttypes.h>
#include "json_serializer.hpp"
#include "log.hpp"

#define INITIAL_JSON_SIZE 1024
#define START_OBJECT_CHAR '{'
#define START_ARRAY_CHAR '['
#define END_OBJECT_CHAR '}'
#define END_ARRAY_CHAR ']'
#define JSON_DELIMITER ','

static appd_iot_error_code_t appd_iot_check_and_expand_json_buf_size(json_t* json, size_t len);
static appd_iot_error_code_t appd_iot_json_start(json_t* json, char begin, const char* name);
static appd_iot_error_code_t appd_iot_json_end(json_t* json, char end);
static appd_iot_error_code_t appd_iot_json_add_key_value(json_t* json,
    const char* key, const void* value, appd_iot_data_types_t type);
static appd_iot_error_code_t appd_iot_json_add_value(json_t* json, const void* value,
    appd_iot_data_types_t type);

static const char* comma = ",";

/**
 * @brief Creates, Initializes and returns a new json struct
 * @return json_t pointer to the newly created json object
 */
json_t* appd_iot_json_init()
{
  json_t* json = (json_t*)calloc(1, sizeof(json_t));

  if (json == NULL)
  {
    return NULL;
  }

  json->buf = (char*)calloc(1, INITIAL_JSON_SIZE + 1);

  if (json->buf == NULL)
  {
    free(json);
    return NULL;
  }

  json->max_len = INITIAL_JSON_SIZE;
  json->last_op = INIT;

  return json;
}

/**
 * @brief starts json blob by adding the begin character.
 * Typically it could be json object '{' or json array '['.
 * If a name is given it will be "name":{ or "name":[
 * @param begin contains the opening character
 * @param name contains the key to json object or array
 * @param json struct which contains the json buf
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t appd_iot_json_start(json_t* json, char begin, const char* name)
{
  if (json == NULL)
  {
    return APPD_IOT_ERR_NULL_PTR;
  }

  if (begin != START_OBJECT_CHAR && begin != START_ARRAY_CHAR)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Unexpected begin character:%c", begin);
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  size_t len = 1; //there will be atleast 1 char and a terminating null character
  const char* eol = &comma[1]; //initialized to null character '/0'

  if (name != NULL)
  {
    len = len + strlen(name) + 3; //2 double quotes and 1 colon "name:"begin
  }

  //add comma at the end if last operation is not start
  if (json->last_op != START_OBJECT && json->last_op != START_ARRAY && json->last_op != INIT)
  {
    len = len + 1;
    eol = &comma[0];
  }

  appd_iot_error_code_t retcode = appd_iot_check_and_expand_json_buf_size(json, len);

  if (retcode != APPD_IOT_SUCCESS)
  {
    return retcode;
  }

  if (name != NULL)
  {
    snprintf(json->buf + json->len, len + 1, "%s\"%s\":%c", eol, name, begin);
  }
  else
  {
    snprintf(json->buf + json->len, len + 1, "%s%c", eol, begin);
  }

  json->len = json->len + len;
  json->buf[json->len] = '\0';

  if (begin == START_ARRAY_CHAR)
  {
    json->last_op = START_ARRAY;
  }
  else if (begin == START_OBJECT_CHAR)
  {
    json->last_op = START_OBJECT;
  }

  return retcode;
}

/**
 * @brief starts json array by adding char '[' to json buf.
 * If a name is given it will be "name":[
 * @param json struct which contains the json buf
 * @param array_name has the string for the key to json array
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_start_array(json_t* json, const char* array_name)
{
  return appd_iot_json_start(json, START_ARRAY_CHAR, array_name);
}

/**
 * @brief starts json object by adding char '{' to json buf.
 * If a name is given it will be "name":{
 * @param json struct which contains the json buf
 * @param object_name has the string for the key to json object
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_start_object(json_t* json, const char* object_name)
{
  return appd_iot_json_start(json, START_OBJECT_CHAR, object_name);
}

/**
 * @brief ends json object with end character. Typically it is either '}' or ']'.
 * @param json contains the json struct to which object end is added
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t appd_iot_json_end(json_t* json, char end)
{
  if (json == NULL)
  {
    return APPD_IOT_ERR_NULL_PTR;
  }

  if (end != END_OBJECT_CHAR && end != END_ARRAY_CHAR)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Unexpected end character:%c", end);
    return APPD_IOT_ERR_INVALID_INPUT;
  }

  //check if space available for end character of size 1 byte
  appd_iot_error_code_t retcode = appd_iot_check_and_expand_json_buf_size(json, 1);

  if (retcode != APPD_IOT_SUCCESS)
  {
    return retcode;
  }

  json->buf[json->len] = end;
  json->len++;

  if (end == END_OBJECT_CHAR)
  {
    json->last_op = END_OBJECT;
  }
  else if (end == END_ARRAY_CHAR)
  {
    json->last_op = END_ARRAY;
  }

  return APPD_IOT_SUCCESS;
}

/**
 * @brief ends json object by adding char '}' to json buf.
 * @param json struct which contains the json buf
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_end_object(json_t* json)
{
  return appd_iot_json_end(json, END_OBJECT_CHAR);
}

/**
 * @brief ends json object by adding char ']' to json buf.
 * @param json struct which contains the json buf
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_end_array(json_t* json)
{
  return appd_iot_json_end(json, END_ARRAY_CHAR);
}

/**
 * @brief adds escape character to the string if applicable.
 * Unicode characters are currently not handled.
 * @param value is the string which is to be escaped.
 * @return std::string contains modified string with escape characters included
 */
std::string appd_iot_add_escape_char(const void* value)
{
  char* temp = (char*)value;
  std::string s = "";


  for (size_t i = 0; i < strlen(temp); i++)
  {
    //Reading only 0 to 127 ascii values. unicode characters are not handled.
    unsigned char t = (temp[i] & 0x7F);

    switch (t)
    {
      case '\b':
        s += "\\b";
        break;

      case '\n':
        s += "\\n";
        break;

      case '\r':
        s += "\\r";
        break;

      case '\t':
        s += "\\t";
        break;

      case '\f':
        s += "\\f";
        break;

      case '"':
        s += "\\\"";
        break;

      case '\\':
        s += "\\\\";
        break;

      case '/':
        s += "\\/";
        break;

      default:
        //if escape sequence not detected, retain the original character
        s.push_back(temp[i]);
        break;
    }
  }

  return s;
}

/**
 * @brief convert integer, double and boolean to string
 * @param buf to which the converted string is to be written
 * @param bufsize indicates the size allocated for the string
 * @param value is the data to be converted to string
 * @param type indicates if the value is string, integer, double or boolean data type
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_convert_to_string
(char* buf, size_t bufsize, const void* value, appd_iot_data_types_t type)
{
  std::string s;

  switch (type)
  {
    case APPD_IOT_STRING:
      s = appd_iot_add_escape_char(value);
      strncpy(buf, s.c_str(), bufsize);
      buf[bufsize - 1] = '\0';
      break;

    case APPD_IOT_INTEGER:
      snprintf(buf, bufsize, "%" PRId64 "", *(int64_t*)value);
      buf[bufsize - 1] = '\0';
      break;

    case APPD_IOT_DOUBLE:
      snprintf(buf, bufsize, "%f", *(double*)value);
      buf[bufsize - 1] = '\0';
      break;

    case APPD_IOT_BOOLEAN:
      snprintf(buf, bufsize, "%s", *(bool*)value ? "true" : "false");
      buf[bufsize - 1] = '\0';
      break;

    default:
      appd_iot_log(APPD_IOT_LOG_WARN, "Invalid Data Type");
      return APPD_IOT_ERR_INVALID_INPUT;
  }

  return APPD_IOT_SUCCESS;
}

/**
 * @brief adds key:value pair to json object
 * @param json struct which contains the json buf
 * @param key to be added
 * @param value to be added
 * @param type indicates if the value is string, integer, double or boolean data type
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t appd_iot_json_add_key_value
(json_t* json, const char* key, const void* value, appd_iot_data_types_t type)
{
  if ((json == NULL) || (key == NULL) || (value == NULL))
  {
    return APPD_IOT_ERR_NULL_PTR;
  }

  char* strval;
  size_t value_size = 256; //default size for integer, double and boolean types

  //for string, set size to twice the original length to accomodate any escape characters.
  if (type == APPD_IOT_STRING)
  {
    value_size = 2 * strlen((char*)value);
  }

  strval = (char*)calloc(1, value_size);

  if (strval == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to allocate memory while adding key-value to json");
    return APPD_IOT_ERR_NULL_PTR;
  }

  appd_iot_error_code_t retcode  = appd_iot_convert_to_string(strval, value_size, value, type);

  if (retcode != APPD_IOT_SUCCESS)
  {
    free(strval);
    return retcode;
  }

  size_t len = strlen(key) + strlen(strval);

  //add space for extra characters doublequote(") and colon(:)
  if (type == APPD_IOT_STRING)
  {
    len = len + 5; //4 doublequotes and 1 colon "key":"value"
  }
  else
  {
    len = len + 3; //2 doublequotes and 1 colon "key":value
  }

  const char* eol = &comma[1]; //initialized to null character '/0'

  //add comma at the end if last operation is not start
  if (json->last_op != START_OBJECT && json->last_op != START_ARRAY && json->last_op != INIT)
  {
    len = len + 1;
    eol = &comma[0];
  }

  retcode = appd_iot_check_and_expand_json_buf_size(json, len);

  if (retcode != APPD_IOT_SUCCESS)
  {
    free(strval);
    return retcode;
  }

  switch (type)
  {
    case APPD_IOT_STRING:
      //snprintf copies string of size 'len' into buffer
      snprintf(json->buf + json->len, len + 1, "%s\"%s\":\"%s\"", eol, key, strval);
      break;

    case APPD_IOT_INTEGER:
    case APPD_IOT_DOUBLE:
    case APPD_IOT_BOOLEAN:
      snprintf(json->buf + json->len, len + 1, "%s\"%s\":%s", eol, key, strval);
      break;

    default:
      appd_iot_log(APPD_IOT_LOG_WARN, "Invalid Data Type");
      free(strval);
      return APPD_IOT_ERR_INVALID_INPUT;

  }

  json->len = json->len + len;
  json->buf[json->len] = '\0';
  json->last_op = ADD_DATA;

  free(strval);

  return retcode;
}

/**
 * @brief adds key:value pair to json object with value as string
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param strval contains string representing value
 * @return appd_iot_error_code_t indicating function execution status
 */

appd_iot_error_code_t appd_iot_json_add_string_key_value
(json_t* json, const char* key, const char* strval)
{
  //do escape, get the size
  return appd_iot_json_add_key_value(json, key, (void*)strval, APPD_IOT_STRING);
}

/**
 * @brief adds key:value pair to json object with value as 64 bit integer
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param intval contains integer representing value
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_integer_key_value(json_t* json, const char* key, int64_t intval)
{
  return appd_iot_json_add_key_value(json, key, (void*)(&intval), APPD_IOT_INTEGER);
}

/**
 * @brief adds double value to json object
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_double_key_value(json_t* json, const char* key, double doubleval)
{
  return appd_iot_json_add_key_value(json, key, (void*)(&doubleval), APPD_IOT_DOUBLE);
}

/**
 * @brief adds key:value pair to json object with value as boolean
 * @param json struct which contains the json buf
 * @param key contains string representing key
 * @param boolval contains boolean representing value
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_boolean_key_value(json_t* json, const char* key, bool boolval)
{
  return appd_iot_json_add_key_value(json, key, (void*)(&boolval), APPD_IOT_BOOLEAN);
}

/**
 * @brief adds value to json object.
 * This function is typically used to add string, integer, double, boolean values to JSON arrays
 * @param json struct which contains the json buf
 * @param value to be added
 * @param type indicates if the value is string, integer, double or boolean data type
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t appd_iot_json_add_value
(json_t* json, const void* value, appd_iot_data_types_t type)
{
  if ((json == NULL) || (value == NULL))
  {
    return APPD_IOT_ERR_NULL_PTR;
  }

  char* strval;
  size_t value_size = 256; //default size for integer, double and boolean types

  //for string, set size to twice the original length to accomodate any escape characters.
  if (type == APPD_IOT_STRING)
  {
    value_size = 2 * strlen((char*)value);
  }

  strval = (char*)calloc(1, value_size);

  if (strval == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to allocate memory while adding key-value to json");
    return APPD_IOT_ERR_NULL_PTR;
  }

  appd_iot_error_code_t retcode  = appd_iot_convert_to_string(strval, value_size, value, type);

  if (retcode != APPD_IOT_SUCCESS)
  {
    free(strval);
    return retcode;
  }

  size_t len = strlen(strval);

  //add space for extra characters doublequote(")
  if (type == APPD_IOT_STRING)
  {
    len = len + 2; //2 doublequotes "value"
  }

  char eol[2];
  eol[0] = '\0';

  //add comma at the end if last operation is not start
  if (json->last_op != START_OBJECT && json->last_op != START_ARRAY && json->last_op != INIT)
  {
    len = len + 1;
    eol[0] = ',';
    eol[1] = '\0';
  }

  retcode = appd_iot_check_and_expand_json_buf_size(json, len);

  if (retcode != APPD_IOT_SUCCESS)
  {
    free(strval);
    return retcode;
  }

  switch (type)
  {
    case APPD_IOT_STRING:
      //snprintf copies string of len into buffer
      snprintf(json->buf + json->len, len + 1, "%s\"%s\"", eol, strval);
      break;

    case APPD_IOT_INTEGER:
    case APPD_IOT_DOUBLE:
    case APPD_IOT_BOOLEAN:
      snprintf(json->buf + json->len, len + 1, "%s%s", eol, strval);
      break;

    default:
      appd_iot_log(APPD_IOT_LOG_WARN, "Invalid Data Type");
      free(strval);
      return APPD_IOT_ERR_INVALID_INPUT;

  }

  json->len = json->len + len;
  json->buf[json->len] = '\0';
  json->last_op = ADD_DATA;

  free(strval);

  return retcode;
}

/**
 * @brief adds value to json object. This function is typically used to add string values to JSON arrays
 * @param json struct which contains the json buf
 * @param value contains string to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_string_value(json_t* json, const char* value)
{
  return appd_iot_json_add_value(json, (void*)value, APPD_IOT_STRING);
}


/**
 * @brief adds value to json object. This function is typically used to add integer values to JSON arrays
 * @param json struct which contains the json buf
 * @param intval contains integer to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_integer_value(json_t* json, int64_t intval)
{
  return appd_iot_json_add_value(json, (void*)(&intval), APPD_IOT_INTEGER);
}

/**
 * @brief adds value to json object. This function is typically used to add double values to JSON arrays
 * @param json struct which contains the json buf
 * @param doubleval contains integer to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_double_value(json_t* json, double doubleval)
{
  return appd_iot_json_add_value(json, (void*)(&doubleval), APPD_IOT_DOUBLE);
}

/**
 * @brief adds value to json object. This function is typically used to add boolean values to JSON arrays
 * @param json struct which contains the json buf
 * @param boolval contains boolean to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
appd_iot_error_code_t appd_iot_json_add_boolean_value(json_t* json, bool boolval)
{
  return appd_iot_json_add_value(json, (void*)(&boolval), APPD_IOT_BOOLEAN);
}

/**
 * @brief returns the json string constructed so far.
 * @param json struct which contains the json buf
 * @return char pointer containing the json string.
 */
const char* appd_iot_json_get_string(json_t* json)
{
  return json->buf;
}

/**
 * @brief format and returns json string with line breaks, indentiation at start/end of objects/arrays
 * @param json struct which contains the json buf
 * @return char pointer containing the json string.
 */
const char* appd_iot_json_pretty_print(json_t* json)
{
  if (json == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "JSON Object is Null");
    return NULL;
  }

  if (json->buf == NULL)
  {
    appd_iot_log(APPD_IOT_LOG_ERROR, "JSON String Buffer is Null");
    return NULL;
  }

  if (json->printbuf != NULL)
  {
    return json->printbuf;
  }

  size_t printbuf_size = 2 * json->len;
  json->printbuf = (char*)calloc(1, printbuf_size);
  size_t printbuf_len = 0;

  int width = 0;

  snprintf(json->printbuf + printbuf_len, 2, "\n");
  printbuf_len += 1;

  for (size_t i = 0; i < json->len; i++)
  {
    if (json->buf[i] == START_ARRAY_CHAR)
    {
      width += 2; //for indentation
    }

    if (json->buf[i] == START_OBJECT_CHAR)
    {
      width += 2; //for indentation
    }

    if (json->buf[i] == END_ARRAY_CHAR)
    {
      width -= 2; //remove indentiation
    }

    if (json->buf[i] == END_OBJECT_CHAR)
    {
      width -= 2; //remove indentiation
    }

    if ((printbuf_len + width + 2) > printbuf_size)
    {
      json->printbuf = (char*)realloc(json->printbuf, printbuf_size * 2);

      if (json->printbuf == NULL)
      {
        appd_iot_log(APPD_IOT_LOG_ERROR, "Failed to Realloc Memory");
        free(json->printbuf);
        return NULL;
      }

      printbuf_size = printbuf_size * 2;
    }

    //print newline and add indentation before closing
    if (json->buf[i] == END_ARRAY_CHAR || json->buf[i] == END_OBJECT_CHAR)
    {
      if (width == 0)
      {
        width = width + 1; //if no indentation, print new line
      }

      snprintf(json->printbuf + printbuf_len, width + 1, "\n%-*s", width, "");
      printbuf_len += width;
    }

    snprintf(json->printbuf + printbuf_len, 2, "%c", json->buf[i]);
    printbuf_len += 1;

    //print newline and add indentation after opening
    if (json->buf[i] == START_ARRAY_CHAR || json->buf[i] == START_OBJECT_CHAR ||
        json->buf[i] == JSON_DELIMITER)
    {
      snprintf(json->printbuf + printbuf_len, width + 1, "\n%-*s", width, "");
      printbuf_len += width;
    }
  }

  json->printbuf[printbuf_len] = '\0';

  return json->printbuf;
}

/**
 * @brief frees json structure
 * @param json struct which contains the json buf
 */
void appd_iot_json_free(json_t* json)
{
  if (json == NULL)
  {
    return;
  }

  if (json->buf != NULL)
  {
    free(json->buf);
  }

  if (json->printbuf != NULL)
  {
    free(json->printbuf);
  }

  free(json);

  return;
}

/**
 * @brief Checks and expands the size of json buf allocated
 * @param json struct which contains the json buf
 * @param len indicates size of new bytes to be added
 * @return appd_iot_error_code_t indicating function execution status
 */
static appd_iot_error_code_t appd_iot_check_and_expand_json_buf_size(json_t* json, size_t len)
{
  if (json == NULL)
  {
    return APPD_IOT_ERR_NULL_PTR;
  }

  size_t new_len = json->len + len;

  if (new_len >= json->max_len)
  {
    size_t newsize = json->max_len * 2;

    while (newsize < new_len)
    {
      newsize = newsize * 2;
    }

    char* tmp = (char*)realloc(json->buf, newsize);

    if (tmp == NULL)
    {
      appd_iot_log(APPD_IOT_LOG_ERROR, "Realloc Failed when expanding json buf size");
      return APPD_IOT_ERR_NULL_PTR;
    }

    json->buf = tmp;
    memset(json->buf + json->max_len, 0, (newsize - json->max_len));
    json->max_len = newsize;
  }

  return APPD_IOT_SUCCESS;
}

