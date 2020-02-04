#cmake_minimum_required(VERSION 3.9)
#project(socket_handler VERSION 0.0.1 LANGUAGES C CXX)
#
## Collect all source files into variable
#file(GLOB_RECURSE SH_SRC src/*.cpp)
#SET(CMAKE_CXX_FLAGS -pthread)
#
## Create major target
#add_library(socket_handler
#        STATIC
#        ${SH_SRC}
#        )
## Lib target alias
#add_library(${API_NAMESPACE_NAME}::socket_handler
#        ALIAS                  socket_handler)
#
#
## Link needed libraries
#target_link_libraries(socket_handler
#        PUBLIC
#        CapnProto::capnp-rpc
#        common
#        )
#add_executable(socket  src/socket.cpp  src/socket_client.cpp src/socket_server.cpp ${SS_SRC})
## Specify include directory
#target_include_directories(socket_handler
#        PUBLIC
#        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#        )
#
#add_subdirectory(test)
cmake_minimum_required(VERSION 3.9)
project(socket_handler VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE SH_SRC src/*.cpp)

# Create major target
add_library(socket_handler
        STATIC
        ${SH_SRC}
        )
# Lib target alias
add_library(${API_NAMESPACE_NAME}::socket_handler
        ALIAS                  socket_handler)

# Link needed libraries
target_link_libraries(socket_handler
        PRIVATE
        nlohmann_json::nlohmann_json
        PUBLIC
        #gen::application_state_management
        CapnProto::capnp-rpc
        common
        )
add_executable(socket  src/socket.cpp  src/socket_client.cpp src/socket_server.cpp ${SS_SRC})


# Specify include directory
target_include_directories(socket_handler
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )


