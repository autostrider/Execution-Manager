cmake_minimum_required(VERSION 3.9)
project (msm VERSION 1.0
            LANGUAGES CXX
            DESCRIPTION "Machine State Manager")

file(GLOB_RECURSE MSM_SRC src/*.cpp)
list(REMOVE_ITEM MSM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/msm_main.cpp)

set(MSM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)

add_executable(msm src/msm_main.cpp ${MSM_SRC})

target_link_libraries(msm
    PUBLIC
      api::machine_state_client
      CapnProto::capnp-rpc
      api::application_state_client
)

target_include_directories(msm
    PUBLIC
      ${MSM_INCLUDE}
)

target_compile_options(msm PRIVATE -Werror -Wall -fdiagnostics-color=always)
target_compile_features(msm PRIVATE cxx_std_14)

add_adaptive_application(
  NAME
    MachineStateManager
  PROCESSES
    msm
)
