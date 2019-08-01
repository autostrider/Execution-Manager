cmake_minimum_required(VERSION 3.9)
project (em_test)

file(GLOB_RECURSE SRCS *.cpp)

add_executable(em_test ${SRCS} ${EM_SRC})

target_include_directories(em_test PRIVATE ${EM_INCLUDE})
target_link_libraries(em_test
  gtest
  gtest_main
)

add_custom_target(em_test_run
  COMMAND em_test `printenv TESTFLAGS`
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "run tests"
)