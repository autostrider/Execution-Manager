cmake_minimum_required(VERSION 3.9)
project(machine_state_client VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE MSC_SRC src/*.cpp)

# Create major target
add_library(machine_state_client
  STATIC
    ${MSC_SRC}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::machine_state_client
            ALIAS                  machine_state_client)

# Link needed libraries
target_link_libraries(machine_state_client
    PUBLIC
      gen::machine_state_management
      CapnProto::capnp-rpc
      common
)

# Specify include directory
target_include_directories(machine_state_client
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
