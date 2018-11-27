cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(appdynamicsiotsdk)

########################################
# Build Settings
########################################

#library will be installed in sdk folder
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

#-Wno-long-long to ignore warning when using PRId64 with ISO C++98 standard gnu_printf
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Werror -Wall -Wno-long-long")

#########################
# Include Directories
#########################

include_directories(include)
 
file(GLOB SOURCES "src/*.cpp")

####################################################
# Target
# sdk : creates sdk dynamics library
####################################################
#Generate the shared library from the sources
add_library(appdynamicsiotsdk SHARED ${SOURCES})

if(BUILD_32BIT)
set_target_properties(appdynamicsiotsdk PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()
 
if(ENABLE_COVERAGE)
set_target_properties(appdynamicsiotsdk PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(appdynamicsiotsdk PROPERTIES LINK_FLAGS "--coverage")
endif()

#Set the location for library installation. Use "sudo make install" to apply
install(TARGETS appdynamicsiotsdk DESTINATION /usr/lib)

######################################
# Copy Headers into include folder
#######################################
add_custom_command(
TARGET appdynamicsiotsdk POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/sdk/include include)
