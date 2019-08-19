cmake_minimum_required(VERSION 3.9)

set(CXX_STANDARD_REQUIRED 14)

add_executable(proc2 src/proc2.cpp)

target_link_libraries(proc2
PRIVATE
api::application_state_client
)

add_executable(proc3 src/proc3.cpp)

target_link_libraries(proc3
  PRIVATE
    api::application_state_client
)

add_adaptive_application(
  NAME
    AdaptiveApplication2
  PROCESSES
    proc2
    proc3
)