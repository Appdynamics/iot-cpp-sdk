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

if(BUILD_TESTS)
add_subdirectory(tests)
endif()
