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

if (BUILD_COVERAGE)

file (GLOB_RECURSE ALL_GCNO_FILES *.gcno)
file (GLOB_RECURSE ALL_GCDA_FILES *.gcda)
file (GLOB_RECURSE ALL_GCOV_FILES *.gcov)
file (GLOB_RECURSE ALL_INFO_FILES *.info)

foreach (gcnofile ${ALL_GCNO_FILES})
    add_custom_command(
    TARGET run-tests POST_BUILD
    COMMAND gcov -s ${CMAKE_SOURCE_DIR}/sdk/src -o ${CMAKE_BINARY_DIR}/sdk/CMakeFiles/appdynamicsiotsdk.dir/src/ ${gcnofile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM)
endforeach()

add_custom_command(
    TARGET run-tests POST_BUILD
    COMMAND lcov --capture --directory . --output-file appd-iot-cpp-coverage-tmp.info
    COMMAND lcov -r appd-iot-cpp-coverage-tmp.info "/usr/include/*" "*/iot-cpp-sdk/sdk/include/*" "*.hpp" --directory . --output-file appd-iot-cpp-coverage.info
    COMMAND genhtml appd-iot-cpp-coverage.info --output-directory out
    COMMAND echo ""
    COMMAND echo "Code Coverage Report is available at ${CMAKE_BINARY_DIR}/out/index.html"
    COMMAND echo ""
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM)

foreach (gcnofile ${ALL_GCNO_FILES})
    file(REMOVE_RECURSE ${gcnofile})
endforeach()

foreach (gcdafile ${ALL_GCDA_FILES})
    file(REMOVE_RECURSE ${gcdafile})
endforeach()

foreach (gcovfile ${ALL_GCOV_FILES})
    file(REMOVE_RECURSE ${gcovfile})
endforeach()

foreach (infofile ${ALL_INFO_FILES})
    file(REMOVE_RECURSE ${infofile})
endforeach()

endif()