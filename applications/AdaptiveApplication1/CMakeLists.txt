cmake_minimum_required(VERSION 3.9)
project(aa1 VERSION 1.0
	    LANGUAGES CXX
	    DESCRIPTION "Test adaptive application 1")

add_executable(proc1 src/aa_main.cpp ${AA1_SRC})

target_link_libraries(proc1
  PRIVATE
    api
    application_state_client
    common
    component_client
    proto_generated
)

target_include_directories(proc1
  PUBLIC
    ${AA1_INCLUDE}
)

target_compile_options(proc1 PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(proc1 PRIVATE cxx_std_14)

add_adaptive_application(
  NAME
    AdaptiveApplication1
  PROCESSES
    proc1
)
