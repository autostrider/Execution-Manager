cmake_minimum_required(VERSION 3.9)
project(common VERSION 0.0.1 LANGUAGES C CXX)

file(GLOB_RECURSE COMMON_SRCS *.cpp)

# Create major target
add_library(common
  STATIC 
    ${COMMON_SRCS}
)

# Specify include directory
target_include_directories(common
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)