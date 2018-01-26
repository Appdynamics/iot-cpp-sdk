cmake_minimum_required(VERSION 3.0)

project (tests)

##########################################
# Download and Build Cgreen for Unit Tests
##########################################

if(!BUILD_TESTS)
MESSAGE(FATAL_ERROR "Build target tests not enabled")
endif()

configure_file(CMakeLists-cgreen.txt.in ${CMAKE_BINARY_DIR}/cgreen/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cgreen)

execute_process(COMMAND ${CMAKE_COMMAND} --build .
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cgreen)

execute_process(COMMAND make WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/cgreen/src")

##########################################
# Build Settings
##########################################
set (APPD_SDK_LINK_LIBS appdynamicsiotsdk)
set (CGREEN_LINK_LIBS cgreen)

##########################################
# Include Directories
##########################################
link_directories (${CMAKE_BINARY_DIR}/sdk/lib ${CMAKE_BINARY_DIR}/cgreen/src/build/src)

include_directories(${CMAKE_SOURCE_DIR}/sdk/include ${CMAKE_SOURCE_DIR}/sdk/src
${CMAKE_BINARY_DIR}/cgreen/src/include)

file(GLOB SOURCES "src/*.cpp")

##########################################
# Target
# tests : create executable
# run-tests : trigger all unit tests
##########################################
add_executable(tests ${SOURCES})

add_dependencies(tests appdynamicsiotsdk)

target_link_libraries(tests ${APPD_SDK_LINK_LIBS} ${CGREEN_LINK_LIBS})

add_custom_target(run-tests COMMAND ./tests)

add_dependencies(run-tests tests appdynamicsiotsdk)
