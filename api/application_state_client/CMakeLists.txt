cmake_minimum_required(VERSION 3.9)
project(application_state_client VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE ASC_SRC src/*.cpp)

# Create major target
add_library(application_state_client 
  STATIC 
    ${ASC_SRC}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::application_state_client 
            ALIAS                  application_state_client)

# Link needed libraries
target_link_libraries(application_state_client
    PRIVATE
      nlohmann_json::nlohmann_json
    PUBLIC
      gen::application_state_management
      CapnProto::capnp-rpc
      # gen::proto_generated
      # ${Protobuf_LIBRARIES}
      common
)
# Specify include directory
target_include_directories(application_state_client
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
