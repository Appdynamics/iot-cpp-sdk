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

#include <cgreen/cgreen.h>
#include "utils.hpp"

using namespace cgreen;

Describe(utils);
BeforeEach(utils) { }
AfterEach(utils) { }

/**
 * @brief Unit Test for Utils Remove Character Function
 */
Ensure(utils, test_utils_remove_character_function)
{
  assert_that(appd_iot_remove_character("abc|def", '|'), is_equal_to_string("abcdef"));
  assert_that(appd_iot_remove_character("|abc|def|", '|'), is_equal_to_string("abcdef"));
  assert_that(appd_iot_remove_character("abc|||def", '|'), is_equal_to_string("abcdef"));
  assert_that(appd_iot_remove_character("abcdef", '|'), is_equal_to_string("abcdef"));
  assert_that(appd_iot_remove_character("abcdef", 'a'), is_equal_to_string("bcdef"));
  assert_that(appd_iot_remove_character("|", '|'), is_equal_to_string(""));
  assert_that(appd_iot_remove_character(NULL, '|'), is_equal_to_string(""));
}


TestSuite* utils_tests()
{

  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, utils, test_utils_remove_character_function);

  return suite;
}
