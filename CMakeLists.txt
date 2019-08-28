cmake_minimum_required(VERSION 3.9)

set(BUILD_TESTING OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# For WSL: implemented signalfd
if(CMAKE_SYSTEM MATCHES "Microsoft$")
  message("WSL is used")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DKJ_USE_EPOLL=0")
endif()

add_subdirectory(tools/json)
add_subdirectory(tools/googletest)
add_subdirectory(tools/capnproto)
add_subdirectory(gen)
add_subdirectory(api)
add_subdirectory(applications)