cmake_minimum_required(VERSION 3.9)
project(persistent_storage VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE PER_SRC src/*.cpp)

# Create major target
add_library(persistent_storage
  STATIC
    ${PER_SRC}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::persistent_storage
            ALIAS                  persistent_storage)

# Link needed libraries
target_link_libraries(persistent_storage
    PRIVATE
      nlohmann_json::nlohmann_json
    PUBLIC
      common
)
# Specify include directory
target_include_directories(persistent_storage
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory(test)