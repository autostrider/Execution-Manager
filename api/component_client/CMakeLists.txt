cmake_minimum_required(VERSION 3.9)
project(component_client VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE CC_SRC src/*.cpp)

# Create major target
add_library(component_client
  STATIC
    ${CC_SRC}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::component_client
            ALIAS                  component_client)

# Link needed libraries
target_link_libraries(component_client
    PUBLIC
      CapnProto::capnp-rpc
      gen::component_state_management
      # ${Protobuf_LIBRARIES}
      # gen::proto_generated
      common
)
# Specify include directory
target_include_directories(component_client
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
