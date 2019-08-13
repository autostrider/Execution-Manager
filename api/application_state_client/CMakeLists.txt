cmake_minimum_required(VERSION 3.9)
project(application_state_client VERSION 0.0.1 LANGUAGES C CXX)

# Collect all generated files into variable
file(GLOB_RECURSE ASC_GEN_FILES gen/*.capnp)
# Collect all source files into variable
file(GLOB_RECURSE ASC_SRC       src/*.cpp)

# If there are generated files compile it with Cap’n Proto tool
if(ASC_GEN_FILES)
  capnp_generate_cpp(ascGenSources ascGenHeaders ${ASC_GEN_FILES})
endif()

# Add custom target to generate files
add_custom_target(application_state_client_gen
  DEPENDS
    ${ascGenSources}
    ${ascGenHeaders}
  COMMENT
    "Generate App state client Cap’n Proto files"
)

# Create major target
add_library(application_state_client STATIC
    ${ASC_SRC}
    ${ascGenSources}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Dependency to generated files
add_dependencies(application_state_client application_state_client_gen)

# Link needed libraries
target_link_libraries(application_state_client
    PUBLIC
      CapnProto::capnp
      CapnProto::capnp-rpc
)
# Specify include directory
target_include_directories(application_state_client
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)