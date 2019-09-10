cmake_minimum_required(VERSION 3.9)
project(aa2 VERSION 1.0
	    LANGUAGES CXX
	    DESCRIPTION "Test adaptive application 2")

file(GLOB_RECURSE AA2_SRC src/*.cpp)
list(REMOVE_ITEM AA2_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/aa_main.cpp)
list(REMOVE_ITEM AA2_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/aa_main_proc3.cpp)
set(AA_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(proc2 src/aa_main.cpp ${AA2_SRC})

target_link_libraries(proc2
  PRIVATE
    api::application_state_client
)

target_include_directories(proc2
  PUBLIC 
    ${AA_INCLUDE}
)

target_compile_options(proc2 PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(proc2 PRIVATE cxx_std_14)

add_executable(proc3 src/aa_main_proc3.cpp ${AA2_SRC})

target_link_libraries(proc3
  PRIVATE
    api::application_state_client
)

target_include_directories(proc3
  PUBLIC 
    ${AA_INCLUDE}
)

target_compile_options(proc3 PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(proc3 PRIVATE cxx_std_14)

add_adaptive_application(
  NAME
    AdaptiveApplication2
  PROCESSES
    proc2
    proc3
)
