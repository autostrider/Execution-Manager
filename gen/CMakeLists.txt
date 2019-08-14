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
      CapnProto::capnp
)

# Specify include directory
target_include_directories(execution_management
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
)

# Dependency to AS and MS
add_dependencies(execution_management application_state_management
                                      machine_state_management
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
  execution_management
)