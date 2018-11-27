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

##########################################
# Target
# run-code-coverage : create a code coverage report
##########################################

if (ENABLE_COVERAGE)
    FIND_PROGRAM(GCOV_PATH gcov)
    FIND_PROGRAM(LCOV_PATH lcov)
    FIND_PROGRAM(GENHTML_PATH genhtml)

    IF(NOT GCOV_PATH)
        MESSAGE(FATAL_ERROR "gcov not found! Aborting...")
    ENDIF() # NOT GCOV_PATH

    IF(NOT LCOV_PATH)
        MESSAGE(FATAL_ERROR "lcov not found! Aborting...")
    ENDIF() # NOT LCOV_PATH

    IF(NOT GENHTML_PATH)
        MESSAGE(FATAL_ERROR "genhtml not found! Aborting...")
    ENDIF() # NOT GENHTML_PATH

add_custom_target(run-code-coverage
    COMMAND lcov --capture --directory . --output-file appd-iot-cpp-coverage-tmp.info
    COMMAND lcov -r appd-iot-cpp-coverage-tmp.info "*/usr/include/*" "*/iot-cpp-sdk/sdk/include/*" "*.hpp" --directory . --output-file appd-iot-cpp-coverage.info
    COMMAND genhtml appd-iot-cpp-coverage.info --output-directory out
    COMMAND echo ""
    COMMAND echo "Code Coverage Report is available at ${CMAKE_BINARY_DIR}/out/index.html"
    COMMAND echo ""
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/appd-iot-cpp-coverage-tmp.info
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/appd-iot-cpp-coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "generating HTML report from lcov files"
    VERBATIM)

add_dependencies(run-code-coverage run-tests tests appdynamicsiotsdk)

file (GLOB_RECURSE ALL_GCNO_FILES *.gcno)
foreach (gcnofile ${ALL_GCNO_FILES})
    add_custom_command(TARGET run-code-coverage PRE_BUILD
    COMMAND echo " .... running gcov ..."
    COMMAND gcov -s ${CMAKE_SOURCE_DIR}/sdk/src -o ${CMAKE_BINARY_DIR}/sdk/CMakeFiles/appdynamicsiotsdk.dir/src/ ${gcnofile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running gcov"
    VERBATIM)
endforeach()

endif()