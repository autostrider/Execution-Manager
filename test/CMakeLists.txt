set(AA1_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../applications/AdaptiveApplication1/include)
file(GLOB_RECURSE AA1_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../applications/AdaptiveApplication1/src/*.cpp)
list(REMOVE_ITEM AA1_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../applications/AdaptiveApplication1/src/aa_main.cpp)

file(GLOB_RECURSE EM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../applications/ExecutionManager/src/*.cpp)
list(REMOVE_ITEM EM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../applications/ExecutionManager/src/em_main.cpp)

file(GLOB_RECURSE MSM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../applications/MachineStateManager/src/*.cpp)
list(REMOVE_ITEM MSM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../applications/MachineStateManager/src/msm_main.cpp)

file(GLOB_RECURSE ASC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/application_state_client/src/*.cpp)
file(GLOB_RECURSE MSC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/machine_state_client/src/*.cpp)

file(GLOB_RECURSE SERVER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/base_server/src/*.cpp)
file(GLOB_RECURSE CLIENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/base_client/src/*.cpp)
file(GLOB_RECURSE SH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/socket_handler/src/*.cpp)
file(GLOB_RECURSE CC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/component/src/*.cpp)
file(GLOB_RECURSE COMMON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../common/other/src/*.cpp)

add_subdirectory(AdaptiveApplication)
add_subdirectory(ApplicationStateClient)
add_subdirectory(Common)
add_subdirectory(ComponentClient)
add_subdirectory(ExecutionManager)
add_subdirectory(MachineStateClient)
add_subdirectory(MachineStateManager)
add_subdirectory(BaseClient)
add_subdirectory(BaseServer)
add_subdirectory(mocks)