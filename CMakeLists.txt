cmake_minimum_required(VERSION 3.0)

project(appdynamicsiotsdk)

######################
# Build Settings
######################
set (CMAKE_CXX_STANDARD 98)

#####################################
# Build Targets sdk, sample and tests
#####################################
add_subdirectory(sdk)
add_subdirectory(sample)

# To build tests, add option flag -DBUILD_TESTS=1 to cmake command
set(BUILD_TESTS, 0)

# To build 32 bit targets on a 64 bit architecture, add option flag -DBUILD_32BIT=1 to cmake command
set(BUILD_32BIT, 0)

#####################################
# Code Coverage for sdk
#####################################
# To run report on code coverage, add option flag -DENABLE_COVERAGE=1 to cmake command
set(ENABLE_COVERAGE, 0)

if (ENABLE_COVERAGE)
set(BUILD_TESTS 1 CACHE STRING "Build the Test Suite")
endif()

if(BUILD_TESTS)
add_subdirectory(tests)
endif()
