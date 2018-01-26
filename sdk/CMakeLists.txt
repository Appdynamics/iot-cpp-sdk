cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(appdynamicsiotsdk)

########################################
# Build Settings
########################################

#library will be installed in sdk folder
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Werror -Wall")

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


 
#Set the location for library installation. Use "sudo make install" to apply
install(TARGETS appdynamicsiotsdk DESTINATION /usr/lib)

######################################
# Copy Headers into include folder
#######################################
add_custom_command(
TARGET appdynamicsiotsdk POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/sdk/include include)
