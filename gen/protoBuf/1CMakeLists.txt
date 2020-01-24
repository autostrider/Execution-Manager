cmake_minimum_required(VERSION 3.9)

set(GEN_NAMESPACE_NAME gen)

find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
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

#################################################################
#                     Encapsulated targets                      #
#################################################################
# Add custom target to generate files
add_custom_target(gen
  COMMENT
    "Generate ProtoBuf files"
)

# Encapsulate generating target
add_dependencies(gen proto_generated)