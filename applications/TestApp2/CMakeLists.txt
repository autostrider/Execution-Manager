cmake_minimum_required(VERSION 3.9)

project (testApp2 VERSION 1.0

                   LANGUAGES CXX

                   DESCRIPTION "Test Application 2")



file(GLOB_RECURSE TEST_SRC src/*.cpp)



add_executable(testApp2 ${TEST_SRC})



target_link_libraries(testApp2

  PRIVATE

    api::application_state_client

    nlohmann_json::nlohmann_json

)



target_compile_options(testApp2 PRIVATE -Werror -Wall -fdiagnostics-color=always)

target_compile_features(testApp2 PRIVATE cxx_std_14)



add_adaptive_application(

  NAME

    TestApplication2

  PROCESSES

    testApp2

)