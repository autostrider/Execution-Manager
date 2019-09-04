cmake_minimum_required(VERSION 3.9)

project (testApp1 VERSION 1.0

                   LANGUAGES CXX

                   DESCRIPTION "Test Application 1")



file(GLOB_RECURSE TEST_SRC src/*.cpp)



add_executable(testApp1 ${TEST_SRC})



target_link_libraries(testApp1

  PRIVATE

    api::application_state_client

    nlohmann_json::nlohmann_json

)



target_compile_options(testApp1 PRIVATE -Werror -Wall -fdiagnostics-color=always)

target_compile_features(testApp1 PRIVATE cxx_std_14)



add_adaptive_application(

  NAME

    TestApplication1

  PROCESSES

    testApp1

)