cmake_minimum_required(VERSION 3.9)
project (socket_handler_test)

set(TEST_NAME base_client)

file(GLOB_RECURSE SRC *.cpp)

add_executable(${TEST_NAME}_test ${SRC} ${CLIENT_SRC})

target_link_libraries(${TEST_NAME}_test
        PUBLIC
            gtest
            gtest_main
            gmock
            api::client
            api::socket_handler
            common
            interfaces
            mock
)

target_compile_definitions(${TEST_NAME}_test PUBLIC -DUNIT_TEST)
add_custom_target(${TEST_NAME}_test_run
        COMMAND ${TEST_NAME}_test `printenv TESTFLAGS`
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Run base client tests"

)

# =============================================
# Coverage
# =============================================
set(COVERAGE_BUILD ${TEST_NAME}_coverage_build)

add_executable(${COVERAGE_BUILD} ${SRC} ${CLIENT_SRC})

target_include_directories(${COVERAGE_BUILD} PRIVATE ${CLIENT_INCLUDE})

target_link_libraries(${COVERAGE_BUILD}
     PUBLIC
        gcov
        gtest
        gtest_main
        gmock
        api::client
        api::socket_handler
        common
        interfaces
        mock
)

target_compile_definitions(${COVERAGE_BUILD} PUBLIC -DUNIT_TEST)
target_compile_options(${COVERAGE_BUILD} PUBLIC -g -O0 -fprofile-arcs -ftest-coverage)
