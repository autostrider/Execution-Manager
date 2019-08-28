cmake_minimum_required(VERSION 3.1)

##
## PROJECT
## name and version
##
project(tinyfsm VERSION 3.7.0 LANGUAGES CXX)

set(TINYFSM_TARGET_NAME ${PROJECT_NAME})

##
## TARGET
## create target and add include path
##
add_library(${TINYFSM_TARGET_NAME} INTERFACE)
#add_library(${PROJECT_NAME}::${TINYFSM_TARGET_NAME} ALIAS ${TINYFSM_TARGET_NAME})

target_include_directories(
    ${TINYFSM_TARGET_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
