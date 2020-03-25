cmake_minimum_required(VERSION 3.9)
project(api VERSION 0.0.1 LANGUAGES C CXX)

set(INTERFACE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create major target
add_library(api INTERFACE)

target_link_libraries(api
    INTERFACE
        common
        gen::proto_generated
        socket_handler
        server
        client
        common
)

# Specify include directory
target_include_directories(api
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)