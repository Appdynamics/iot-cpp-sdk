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

#include <cgreen/cgreen.h>
#include "json_serializer.hpp"

using namespace cgreen;

Describe(json_serializer);
BeforeEach(json_serializer) { }
AfterEach(json_serializer) { }

/*
 * Test Cases for JSON Serializer.
 * Sample JSON for Test Cases picked up from:
 * https://github.com/json-schema-org/JSON-Schema-Test-Suite/blob/master/tests/draft6
 */

/*
 * @brief Test for different JSON data types
 * TEST CASES:
 * {"data": "foo"}
 * {"data": 1}
 * {"data": 0.0075}
 * {"data": true}
 */
Ensure(json_serializer, test_json_add_single_value)
{
  const char* input[] =
  {
    "{\"data\":\"foo\"}",
    "{\"data\":9223372036854775807}",
    "{\"data\":92233727.188780}",
    "{\"data\":true}"
  };

  const char* output;
  json_t* json;

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "data", "foo");
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[0], output), is_equal_to(0));
  appd_iot_json_free(json);
  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_integer_key_value(json, "data", 9223372036854775807);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[1], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_double_key_value(json, "data", 92233727.188780);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[2], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_boolean_key_value(json, "data", true);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[3], output), is_equal_to(0));
  appd_iot_json_free(json);
}

/**
 * @brief Test for different JSON nesting
 * TEST CASES:
 * {"data": [1, 2, "foo", true]}
 * {"data": [{"foo": "bar"}, {"foo": "baz"}]}
 * {"data": [{"foo": {"bar" : {"baz" : false}}}]}
 * {"value": 2, "subtree": {"meta": "child", "nodes": [{"value": 2.1},{"value": 2.2}]}}
 * //TODO - add below test cases
 * {"data": {}}
 * {"data": {"list": ["a"]}}
 * {"data": { "foo": []}}
 * {"data": { "answer 1": "42" }}
 * {"data": { "a_x_3": 3 }}
 * {"data": ""}
 * {"data": []}
 * {"data": [1]}
 * {"data": {"0": "invalid", "1": "valid", "length": 2}},
 * {"data": {"definitions": {"foo": {"type": "integer"}}}}
 * {"data": {"properties": {"bar": {"type": "string","minLength": 4, "default": "bad"}}}}
 */
Ensure(json_serializer, test_json_nesting)
{
  const char* input[] =
  {
    "{\"data\":[1,2,3.654654,\"foo\",true]}",
    "{\"data\":[{\"foo\":\"bar\"},{\"foo\":\"baz\"}]}",
    "{\"data\":[{\"foo\":{\"bar\":\{\"baz\":false}}}]}",
    "{\"value\":2,\"subtree\":{\"meta\":\"child\",\"nodes\":[{\"value\":2.124124},{\"value\":2.234234}]}}"
  };

  const char* output;
  json_t* json;

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_start_array(json, "data");
  appd_iot_json_add_integer_value(json, 1);
  appd_iot_json_add_integer_value(json, 2);
  appd_iot_json_add_double_value(json, 3.654654);
  appd_iot_json_add_string_value(json, "foo");
  appd_iot_json_add_boolean_value(json, true);
  appd_iot_json_end_array(json);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[0], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_start_array(json, "data");
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "foo", "bar");
  appd_iot_json_end_object(json);
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "foo", "baz");
  appd_iot_json_end_object(json);
  appd_iot_json_end_array(json);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[1], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_start_array(json, "data");
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_start_object(json, "foo");
  appd_iot_json_start_object(json, "bar");
  appd_iot_json_add_boolean_key_value(json, "baz", false);
  appd_iot_json_end_object(json);
  appd_iot_json_end_object(json);
  appd_iot_json_end_object(json);
  appd_iot_json_end_array(json);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[2], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_integer_key_value(json, "value", 2);
  appd_iot_json_start_object(json, "subtree");
  appd_iot_json_add_string_key_value(json, "meta", "child");
  appd_iot_json_start_array(json, "nodes");
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_double_key_value(json, "value", 2.124124);
  appd_iot_json_end_object(json);
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_double_key_value(json, "value", 2.234234);
  appd_iot_json_end_object(json);
  appd_iot_json_end_array(json);
  appd_iot_json_end_object(json);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[3], output), is_equal_to(0));
  appd_iot_json_free(json);

}

/**
 * @brief Test for different characters/symbols
 * Test Cases:
 * {"data": {"$ref": "a"}}
 * { "data": "1963-06-19T08:30:06.283185Z"}
 * //TODO - add below test cases
 * {"data": "06/19/1963 08:30:06 PST"}
 * {"data": "2013-350T01:01:01"}
 * {"data": "/foo/bar~0/baz~1/%a"},
 * {"data": "/i\\j"},
 * {"data": "/foo/-/bar"},
 * {"data": "^\\S(|(.|\\n)*\\S)\\Z"}
 */
Ensure(json_serializer, test_json_symbols)
{
  const char* input[] =
  {
    "{\"data\":{\"$ref\":\"a\"}}",
    "{\"data\":\"1963-06-19T08:30:06.283185Z\"}"
  };

  const char* output;
  json_t* json;

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_start_object(json, "data");
  appd_iot_json_add_string_key_value(json, "$ref", "a");
  appd_iot_json_end_object(json);
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[0], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "data", "1963-06-19T08:30:06.283185Z");
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[1], output), is_equal_to(0));
  appd_iot_json_free(json);
}

/**
 * @brief Test for different URL structure
 * TEST CASES:
 * {"data": "http://foo.bar/?baz=qux#quux"},
 * {"data": "http://foo.bar/?q=Test%20URL-encoded%20stuff"},
 * {"data": "http://foo.com/blah_(wikipedia)_blah#cite-1"},
 * //TODO - add below test cases
 * {"data": "http://xn--nw2a.xn--j6w193g/"},
 * {"data": "http://-.~_!$&'()*+,;=:%40:80%2f::::::@example.com"},
 * {"data": "http://223.255.255.254"},
 * {"data": "ftp://ftp.is.co.za/rfc/rfc1808.txt"},
 * {"data": "ldap://[2001:db8::7]/c=GB?objectClass?one"},
 * {"data": "mailto:John.Doe@example.com"},
 * {"data": "urn:oasis:names:specification:docbook:dtd:xml:4.1.2"},
 * {"data": "\\\\WINDOWS\\fileshare"},
 * {"data": "http://example.com/dictionary/{term:1}/{term}"},
 */
Ensure(json_serializer, test_json_url)
{
  const char* input[] =
  {
    "{\"data\":\"http:\\/\\/foo.bar\\/?baz=qux#quux\"}",
    "{\"data\":\"http:\\/\\/foo.bar\\/?q=Test%20URL-encoded%20stuff\"}",
    "{\"data\":\"http:\\/\\/foo.com\\/blah_(wikipedia)_blah#cite-1\"}",
  };

  const char* output;
  json_t* json;

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "data", "http://foo.bar/?baz=qux#quux");
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[0], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "data", "http://foo.bar/?q=Test%20URL-encoded%20stuff");
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[1], output), is_equal_to(0));
  appd_iot_json_free(json);

  json = appd_iot_json_init();
  appd_iot_json_start_object(json, NULL);
  appd_iot_json_add_string_key_value(json, "data", "http://foo.com/blah_(wikipedia)_blah#cite-1");
  appd_iot_json_end_object(json);
  output =  appd_iot_json_get_string(json);
  assert_that(strcmp(input[2], output), is_equal_to(0));
  appd_iot_json_free(json);
}

TestSuite* json_serializer_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, json_serializer, test_json_add_single_value);
  add_test_with_context(suite, json_serializer, test_json_nesting);
  add_test_with_context(suite, json_serializer, test_json_symbols);
  add_test_with_context(suite, json_serializer, test_json_url);

  return suite;
}

