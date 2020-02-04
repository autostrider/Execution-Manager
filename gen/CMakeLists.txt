# CMake gen namespace e.g. gen::<some_service_lib>
set(GEN_NAMESPACE_NAME gen)
set(CAPNPC_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)

#################################################################
#                 Application State Management                  #
#################################################################
capnp_generate_cpp(asGenSources asGenHeaders
  application_state_management.capnp
)

# Create major target
add_library(application_state_management STATIC
   ${asGenSources}
   ${asGenHeaders}
)
# Lib target alias
add_library(${GEN_NAMESPACE_NAME}::application_state_management
            ALIAS                  application_state_management)

# Link needed libraries
target_link_libraries(application_state_management
    PUBLIC
      CapnProto::capnp
)

# Specify include directory
target_include_directories(application_state_management
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
)

#################################################################
#                   Machine State Management                    #
#################################################################
capnp_generate_cpp(msGenSources msGenHeaders
  machine_state_management.capnp
)

# Create major target
add_library(machine_state_management STATIC
   ${msGenSources}
   ${msGenHeaders}
)
# Lib target alias
add_library(${GEN_NAMESPACE_NAME}::machine_state_management
            ALIAS                  machine_state_management)

# Link needed libraries
target_link_libraries(machine_state_management
    PUBLIC
      CapnProto::capnp
)

# Specify include directory
target_include_directories(machine_state_management
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
)

#################################################################
#                     Component State Management                #
#################################################################
capnp_generate_cpp(cmGenSources cmGenHeaders
  component_state_management.capnp
)

# Create major target
add_library(component_state_management STATIC
   ${cmGenSources}
   ${cmGenHeaders}
)
# Lib target alias
add_library(${GEN_NAMESPACE_NAME}::component_state_management
            ALIAS                  component_state_management)

# Link needed libraries
target_link_libraries(component_state_management
    PUBLIC
      CapnProto::capnp
)

# Specify include directory
target_include_directories(component_state_management
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
)

#################################################################
#                     Execution Management                      #
#################################################################
capnp_generate_cpp(emGenSources emGenHeaders
  execution_management.capnp
)

# Create major target
add_library(execution_management STATIC
   ${emGenSources}
   ${emGenHeaders}
)
# Lib target alias
add_library(${GEN_NAMESPACE_NAME}::execution_management
            ALIAS                  execution_management)

# Link needed libraries
target_link_libraries(execution_management
    PUBLIC
      machine_state_management
      application_state_management
      component_state_management
      CapnProto::capnp
)

# Specify include directory
target_include_directories(execution_management
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
)

# Dependency to AS, MS and CM
add_dependencies(execution_management application_state_management
                                      machine_state_management
                                      component_state_management
)

#################################################################
#                            ProtoBuf                           #
#################################################################
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
    "Generate Cap’n Proto files"
)

# Encapsulate all generating targets
add_dependencies(gen
  machine_state_management
  application_state_management
  component_state_management
  execution_management
  proto_generated
)