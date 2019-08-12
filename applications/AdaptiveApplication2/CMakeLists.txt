cmake_minimum_required(VERSION 3.9)
project(aa VERSION 1.0
	   LANGUAGES CXX
	   DESCRIPTION "Test adaptive application 2")


add_executable(proc2 src/proc2.cpp)
add_executable(proc3 src/proc3.cpp)

target_compile_options(aa PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(aa PRIVATE cxx_std_14)
