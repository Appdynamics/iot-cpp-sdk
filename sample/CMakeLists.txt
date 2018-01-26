cmake_minimum_required(VERSION 3.0)

project (sample)

######################
# Build Settings
######################
#For the shared library:
set (APPD_SDK_LINK_LIBS appdynamicsiotsdk)

##############################
# Include and Link Directories
##############################
include_directories(${CMAKE_SOURCE_DIR}/sdk/include)

link_directories(${CMAKE_BINARY_DIR}/sdk/lib)

file(GLOB SOURCES "src/*.cpp")
######################################################
# Build Target
# sample : create executable to run sample application
######################################################
add_executable(sample ${SOURCES})

add_dependencies(sample appdynamicsiotsdk)

target_link_libraries(sample ${APPD_SDK_LINK_LIBS} curl)
