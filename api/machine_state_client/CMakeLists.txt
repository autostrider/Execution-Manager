cmake_minimum_required(VERSION 3.9)

project(machine_state_client VERSION 0.0.1 LANGUAGES C CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${API_NAMESPACE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)