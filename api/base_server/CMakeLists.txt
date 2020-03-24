cmake_minimum_required(VERSION 3.9)
project(server VERSION 0.0.1 LANGUAGES C CXX)

set(SERVER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect all source files into variable
file(GLOB_RECURSE SERVER_SRC src/*.cpp)

# Create major target
add_library(server
        STATIC
           ${SERVER_SRC}
)

# Lib target alias
add_library(${API_NAMESPACE_NAME}::server
            ALIAS                  server)

# Link needed libraries
target_link_libraries(server
        PUBLIC
          common
          interfaces
          socket_handler
          client
)

# Specify include directory
target_include_directories(server
        PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

