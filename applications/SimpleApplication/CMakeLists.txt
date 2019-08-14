cmake_minimum_required(VERSION 3.9)
project (simpleApp VERSION 1.0
            LANGUAGES CXX
            DESCRIPTION "Simple Application")

file(GLOB_RECURSE SIMPLE_SRC src/*.cpp)

add_executable(simpleApp ${SIMPLE_SRC})

target_link_libraries(simpleApp
    PRIVATE
        api::application_state_client
)

# target_include_directories(simpleApp
#     PRIVATE
#       ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

target_compile_options(simpleApp PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(simpleApp PRIVATE cxx_std_14)
