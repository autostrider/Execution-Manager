cmake_minimum_required(VERSION 3.1)

##
## PROJECT
## name and version
##
project(nlohmann_json VERSION 3.7.0 LANGUAGES CXX)

set(NLOHMANN_JSON_TARGET_NAME ${PROJECT_NAME})

##
## TARGET
## create target and add include path
##
add_library(${NLOHMANN_JSON_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${NLOHMANN_JSON_TARGET_NAME} ALIAS ${NLOHMANN_JSON_TARGET_NAME})

target_include_directories(
    ${NLOHMANN_JSON_TARGET_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)