cmake_minimum_required(VERSION 3.9)
project(interfaces VERSION 0.0.1 LANGUAGES C CXX)

set(INTERF_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create major target
add_library(interfaces INTERFACE)

target_link_libraries(interfaces
    INTERFACE
        common
)

# Specify include directory
target_include_directories(interfaces
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)