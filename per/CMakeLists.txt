cmake_minimum_required(VERSION 3.9)
project(per_memory VERSION 0.0.1 LANGUAGES C CXX)

# Collect all source files into variable
file(GLOB_RECURSE PM_SRC src/*.cpp)

# Create major target
add_library(per_memory
        STATIC
        ${PM_SRC}
)

# Lib target alias
add_library(per::per_memory
        ALIAS                  per_memory)

# Link needed libraries
target_link_libraries(per_memory
        PUBLIC
   common
   nlohmann_json::nlohmann_json
)

# Specify include directory
target_include_directories(per_memory
        PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)