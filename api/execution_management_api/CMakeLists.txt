cmake_minimum_required(VERSION 3.9)
project(execution_management_api VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE EM_API_SRC       src/*.cpp)

# Create major target
add_library(execution_management_api 
  STATIC
   ${EM_API_SRC}
)
# Lib target alias
add_library(${API_NAMESPACE_NAME}::execution_management_api 
            ALIAS                  execution_management_api)

# Link needed libraries
target_link_libraries(execution_management_api
    PUBLIC
      gen::execution_management
      CapnProto::capnp-rpc
)
# Specify include directory
target_include_directories(execution_management_api
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)