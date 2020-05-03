cmake_minimum_required(VERSION 3.9)
project(common VERSION 0.0.1 LANGUAGES C CXX)

file(GLOB_RECURSE COMMON_SRCS *.cpp)
set(COMMON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(REMOVE_ITEM COMMON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/test/)


# Create major target
add_library(common
  STATIC
    ${COMMON_SRCS}
)

target_link_libraries(common
    PUBLIC
      nlohmann_json::nlohmann_json
      gen::application_state_management
)

# Specify include directory
target_include_directories(common
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${MSM_INCLUDE}
      ${EM_INCLUDE}
)

target_compile_options(common PRIVATE -Werror -Wall -fdiagnostics-color=always -fPIC)
target_compile_features(common PRIVATE cxx_std_14)

