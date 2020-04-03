# CMake gen namespace e.g. gen::<some_service_lib>
set(GEN_NAMESPACE_NAME gen)
set(CAPNPC_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
set(PROTO_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build/gen)

#################################################################
#                            ProtoBuf                           #
#################################################################
find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
      any.proto
      application_state_management.proto
      enums.proto
      component_state_management.proto
      execution_management.proto
      machine_state_management.proto)

add_library(proto_generated STATIC
   ${PROTO_SRCS}
   ${PROTO_HDRS}
)

# Lib target alias
add_library(${GEN_NAMESPACE_NAME}::proto_generated
            ALIAS                  proto_generated)

# Link needed libraries
target_link_libraries(proto_generated
      ${Protobuf_LIBRARIES}
)

# Specify include directory
target_include_directories(proto_generated
    PUBLIC
      $<BUILD_INTERFACE:${PROTO_OUTPUT_DIR}>
      $<INSTALL_INTERFACE:${PROTO_OUTPUT_DIR}>
)

#################################################################
#                     Encapsulated targets                      #
#################################################################
# Add custom target to generate files
add_custom_target(gen
  COMMENT
    "Generate Proto files"
)

# Encapsulate all generating targets
add_dependencies(gen
  proto_generated
)