cmake_minimum_required(VERSION 3.9)
project(client VERSION 0.0.1 LANGUAGES C CXX)

set(CLIENT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect all source files into variable
file(GLOB_RECURSE CLIENT_SRC src/*.cpp)

# Create major target
add_library(client
        STATIC
           ${CLIENT_SRC}
)

# Lib target alias
add_library(${BASE_CLIENT_NAMESPACE_NAME}::client
            ALIAS                          client)

# Link needed libraries
target_link_libraries(client
        PUBLIC
          common
          api
          socket_handler
)

# Specify include directory
target_include_directories(client
        PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

