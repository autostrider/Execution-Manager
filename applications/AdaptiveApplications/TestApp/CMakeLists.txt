cmake_minimum_required(VERSION 3.9)
project(aa1 VERSION 1.0
	    LANGUAGES CXX
	    DESCRIPTION "Test adaptive application 1")

file(GLOB_RECURSE AA1_SRC src/*.cpp)
list(REMOVE_ITEM AA1_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/aa_main.cpp)
set(AA_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(aa1 src/aa_main.cpp ${AA1_SRC})

target_include_directories(aa1
    PUBLIC
        ${AA_INCLUDE}
)

target_link_libraries(
    aa1
    PUBLIC
    tinyfsm
    )

target_compile_options(aa1 PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(aa1 PRIVATE cxx_std_14)
