cmake_minimum_required(VERSION 3.9)
project(proc1 VERSION 1.0
	    LANGUAGES CXX
	    DESCRIPTION "Test adaptive application 1")

file(GLOB_RECURSE AA1_SRC src/*.cpp)
list(REMOVE_ITEM AA1_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/aa_main.cpp)

add_executable(proc1 src/aa_main.cpp ${AA1_SRC})

target_compile_options(proc1 PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(proc1 PRIVATE cxx_std_14)
