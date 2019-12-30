cmake_minimum_required(VERSION 3.9)
project(common VERSION 0.0.1 LANGUAGES C CXX)

add_subdirectory(test)
file(GLOB_RECURSE COMMON_SRCS *.cpp)
set(COMMON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(REMOVE_ITEM COMMON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/mocks)
list(REMOVE_ITEM COMMON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/test/)


# Create major target
add_library(common
  STATIC
    ${COMMON_SRCS}
)

target_link_libraries(common
    PUBLIC
      api::application_state_client
      api::machine_state_client
      api::component_client
      per::per_memory
      nlohmann_json::nlohmann_json
      gmock
)

# Specify include directory
target_include_directories(common
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${MSM_INCLUDE}
      ${EM_INCLUDE}
)
