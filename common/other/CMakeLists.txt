cmake_minimum_required(VERSION 3.9)
project(common VERSION 0.0.1 LANGUAGES C CXX)

file(GLOB_RECURSE COMMON_SRCS *.cpp)
set(COMMON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)


# Create major target
add_library(common
  STATIC
    ${COMMON_SRCS}
)

target_link_libraries(common
    PUBLIC
      proto_generated
      per::per_memory
      nlohmann_json::nlohmann_json
      api
      mock
)

# Specify include directory
target_include_directories(common
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${MSM_INCLUDE}
      ${EM_INCLUDE}
)
