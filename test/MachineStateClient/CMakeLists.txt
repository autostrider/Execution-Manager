cmake_minimum_required(VERSION 3.9)
project (machine_state_client_test)

set(TEST_NAME machine_state_client)

file(GLOB_RECURSE SRCS *.cpp)

add_executable(${TEST_NAME}_test ${SRCS} ${MSC_SRC})

target_link_libraries(${TEST_NAME}_test
  gmock
  gtest
  gtest_main
  api::machine_state_client
  gen::machine_state_management
  gen::execution_management
  # gen::proto_generated
)

target_compile_definitions(${TEST_NAME}_test PUBLIC -DUNIT_TEST)
add_custom_target(${TEST_NAME}_test_run
  COMMAND ${TEST_NAME}_test `printenv TESTFLAGS`
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Run MSC tests"
)

# =============================================
# Coverage
# =============================================
set(COVERAGE_BUILD ${TEST_NAME}_coverage_build)

add_executable(${COVERAGE_BUILD} ${SRCS} ${MSC_SRC})

target_link_libraries(${COVERAGE_BUILD}
  gcov
  gmock
  gtest
  gtest_main
  api::machine_state_client
  gen::machine_state_management
  gen::execution_management
  # gen::proto_generated
)

target_compile_definitions(${COVERAGE_BUILD} PUBLIC -DUNIT_TEST)
target_compile_options(${COVERAGE_BUILD} PUBLIC -g -O0 -fprofile-arcs -ftest-coverage)
