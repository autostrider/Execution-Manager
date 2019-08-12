cmake_minimum_required(VERSION 3.9)
project(execution_management_api VERSION 0.0.1 LANGUAGES C CXX)

# Collect all generated files into variable
file(GLOB_RECURSE EM_API_GEN_FILES gen/*.capnp)
# Collect all source files into variable
file(GLOB_RECURSE EM_API_SRC       src/*.cpp)

# If there are generated files compile it with Cap’n Proto tool
if(EM_API_GEN_FILES)
  capnp_generate_cpp(emApiGenSources emApiGenHeaders ${EM_API_GEN_FILES})
endif()

# Add custom target to generate files
add_custom_target(execution_management_api_gen
  DEPENDS
    ${emApiGenSources}
    ${emApiGenHeaders}
  COMMENT
    "Generate EM::API Cap’n Proto files"
)

# Create major target
add_library(execution_management_api STATIC
  ${EM_API_SRC}
  ${emApiGenSources}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Dependency to generated files
add_dependencies(execution_management_api execution_management_api_gen)

# Link needed libraries
target_link_libraries(execution_management_api
    PUBLIC
      CapnProto::capnp
      CapnProto::capnp-rpc
)
# Specify include directory
target_include_directories(execution_management_api
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)