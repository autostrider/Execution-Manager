cmake_minimum_required(VERSION 3.9)

set(CXX_STANDARD_REQUIRED 14)

add_executable(proc2 src/proc2.cpp ${AA1_SRC})

target_link_libraries(proc2
  PRIVATE
    proto_generated
)
target_include_directories(proc2
  PUBLIC
    ${AA1_INCLUDE}
)

add_executable(proc3 src/proc3.cpp ${AA1_SRC})

target_link_libraries(proc3
  PRIVATE
    proto_generated
)

target_include_directories(proc3
  PUBLIC
    ${AA1_INCLUDE}
)

add_adaptive_application(
  NAME
    AdaptiveApplication2
  PROCESSES
    proc2
    proc3
)