set(AA1_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/AdaptiveApplication1/include)
file(GLOB_RECURSE AA1_SRC ${CMAKE_CURRENT_SOURCE_DIR}/AdaptiveApplication1/src/*.cpp)
list(REMOVE_ITEM AA1_SRC ${CMAKE_CURRENT_SOURCE_DIR}/AdaptiveApplication1/src/aa_main.cpp)

include(AdaptiveApplication.cmake)
add_subdirectory(ExecutionManager)
add_subdirectory(MachineStateManager)
add_subdirectory(AdaptiveApplication1)
#add_subdirectory(AdaptiveApplication2)
