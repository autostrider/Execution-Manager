cmake_minimum_required(VERSION 3.9)
project (socket_handler_test)

set(TEST_NAME socket_handler)

file(GLOB_RECURSE SRCS *.cpp)

add_executable(${TEST_NAME}_test ${SRCS} ${SH_SRC} socket_handler_test.cpp)

target_link_libraries(${TEST_NAME}_test
        gmock
        gtest
        gtest_main
        common
        api::machine_state_client
        gen::machine_state_management
        gen::execution_management
        )

target_compile_definitions(${TEST_NAME}_test PUBLIC -DUNIT_TEST)
add_custom_target(${TEST_NAME}_test_run
        COMMAND ${TEST_NAME}_test `printenv TESTFLAGS`
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Run SH tests"
        )