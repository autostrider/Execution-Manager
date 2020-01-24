cmake_minimum_required(VERSION 3.9)
project (cc_test)

set(TEST_NAME cc)

file(GLOB_RECURSE SRCS *.cpp)

add_executable(${TEST_NAME}_test ${SRCS} ${CC_SRC})

target_include_directories(${TEST_NAME}_test PRIVATE ${EM_INCLUDE})
target_link_libraries(${TEST_NAME}_test
  gtest
  gtest_main
  gmock
  api::application_state_client
  api::component_client
  api::machine_state_client
  gen::execution_management
  CapnProto::capnp-rpc
  # gen::proto_generated
  # ${Protobuf_LIBRARIES}
  common
)

target_compile_definitions(${TEST_NAME}_test PUBLIC -DUNIT_TEST)
add_custom_target(${TEST_NAME}_test_run
  COMMAND ${TEST_NAME}_test `printenv TESTFLAGS`
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Run component client tests"
)

# =============================================
# Coverage
# =============================================
set(COVERAGE_BUILD ${TEST_NAME}_coverage_build)

add_executable(${COVERAGE_BUILD} ${SRCS} ${CC_SRC})

target_include_directories(${COVERAGE_BUILD} PRIVATE ${EM_INCLUDE})
target_link_libraries(${COVERAGE_BUILD}
  gcov
  gtest
  gtest_main
  gmock
  api::application_state_client
  api::component_client
  api::machine_state_client
  gen::execution_management
  CapnProto::capnp-rpc
  # gen::proto_generated
  # ${Protobuf_LIBRARIES}
  common
)

target_compile_definitions(${COVERAGE_BUILD} PUBLIC -DUNIT_TEST)
target_compile_options(${COVERAGE_BUILD} PUBLIC -g -O0 -fprofile-arcs -ftest-coverage)
