cmake_minimum_required(VERSION 3.9)
project(mocks VERSION 0.0.1 LANGUAGES C CXX)

file(GLOB_RECURSE MOCKS_SRCS *.cpp)
set(MOCKS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create major target
add_library(mock
  STATIC
    ${MOCKS_SRCS}
)

target_link_libraries(mock
    PUBLIC
      application_state::application_state_client
      machine_state_client::machine_state_client
      component_client::component_client
      per::per_memory
      nlohmann_json::nlohmann_json
      gmock
      api
)

# Specify include directory
target_include_directories(mock
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${MSM_INCLUDE}
      ${EM_INCLUDE}
)