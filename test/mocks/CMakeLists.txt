cmake_minimum_required(VERSION 3.9)
project(mockes VERSION 0.0.1 LANGUAGES C CXX)

file(GLOB_RECURSE MOCKS_SRCS *.cpp)

set(MOCKS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

