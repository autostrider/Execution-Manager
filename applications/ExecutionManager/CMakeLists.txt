cmake_minimum_required(VERSION 3.9)
project (em VERSION 1.0
            LANGUAGES CXX
            DESCRIPTION "Execution Manager")

file(GLOB_RECURSE EM_SRC src/*.cpp)
list(REMOVE_ITEM EM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/em_main.cpp)

add_executable(em src/em_main.cpp ${EM_SRC})

target_link_libraries(em
    PUBLIC
    nlohmann_json::nlohmann_json
      gen::execution_management
      application_state::application_state_client
      CapnProto::capnp-rpc
      common
      socket_handler
)

target_include_directories(em
    PUBLIC
      ${EM_INCLUDE}
)

target_compile_options(em PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(em PRIVATE cxx_std_14)
