cmake_minimum_required(VERSION 3.9)
project (manifest_parser VERSION 1.0
                         LANGUAGES CXX
                         DESCRIPTION "Manifest Parser")

set(MP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)

add_library(manifest_parser INTERFACE)

target_link_libraries(manifest_parser INTERFACE nlohmann_json::nlohmann_json)
target_include_directories(manifest_parser
    INTERFACE
        ${MP_INCLUDE}
)

target_compile_options(manifest_parser INTERFACE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(manifest_parser INTERFACE cxx_std_14)