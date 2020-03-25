cmake_minimum_required(VERSION 3.9)
project(socket_handler VERSION 0.0.1 LANGUAGES C CXX)

set(SH_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect all source files into variable
file(GLOB_RECURSE SH_SRC src/*.cpp)

# Create major target
add_library(socket_handler
        STATIC
           ${SH_SRC}
)

# Lib target alias
add_library(${API_NAMESPACE_NAME}::socket_handler
            ALIAS                  socket_handler)

# Link needed libraries
target_link_libraries(socket_handler
        PUBLIC
          common
          interfaces
)

# Specify include directory
target_include_directories(socket_handler
        PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# =============================================
# Coverage
# =============================================
set(COVERAGE_BUILD ${TEST_NAME}_coverage_build)

add_executable(${COVERAGE_BUILD} ${SRC} ${SH_SRC})

target_include_directories(${COVERAGE_BUILD} PRIVATE ${SH_INCLUDE})

target_link_libraries(${COVERAGE_BUILD}
     PUBLIC
        gcov
        gtest
        gtest_main
        gmock
        api::socket_handler
        common
        interfaces
        mock
)

target_compile_definitions(${COVERAGE_BUILD} PUBLIC -DUNIT_TEST)
target_compile_options(${COVERAGE_BUILD} PUBLIC -g -O0 -fprofile-arcs -ftest-coverage)
