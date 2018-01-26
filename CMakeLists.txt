cmake_minimum_required(VERSION 3.0)

project(appdynamicsiotsdk)

#####################################
# Build Targets sdk, sample and tests
#####################################

add_subdirectory(sdk)
add_subdirectory(sample)

# To build tests, add option flag -DBUILD_TESTS=1 to cmake command
set(BUILD_TESTS, 0)

if(BUILD_TESTS)
add_subdirectory(tests)
endif()
