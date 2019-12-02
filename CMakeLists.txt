cmake_minimum_required(VERSION 3.9)

set(BUILD_TESTING OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(MSM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/applications/MachineStateManager/include)
set(EM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/applications/ExecutionManager/include)

find_program(GCOV_PATH gcov)
if(NOT GCOV_PATH)
   message(FATAL_ERROR "gcov not found! Aborting...")
endif()

# For WSL: implemented signalfd
if(CMAKE_SYSTEM MATCHES "Microsoft$")
  message("WSL is used")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DKJ_USE_EPOLL=0")
endif()

add_subdirectory(tools/json)
add_subdirectory(tools/googletest)
add_subdirectory(tools/capnproto)
add_subdirectory(gen)
add_subdirectory(api)
add_subdirectory(applications)
add_subdirectory(common)

# =============================================
# Coverage
# =============================================
set(LCOV_PATH ${CMAKE_SOURCE_DIR}/tools/lcov/bin/lcov)
set(GENHTML_PATH ${CMAKE_SOURCE_DIR}/tools/lcov/bin/genhtml)
set(COVERAGE_LCOV_EXCLUDES '${CMAKE_SOURCE_DIR}/tools/*' '${CMAKE_SOURCE_DIR}/gen/*' '/usr/include/*')
set(COVERAGE_REPORT_DIR ${CMAKE_CURRENT_BINARY_DIR}/coverage_report)

add_custom_target(coverage
  COMMAND em_coverage_build
  COMMAND msm_coverage_build
  COMMAND machine_state_client_coverage_build
  COMMAND adaptiveApp_coverage_build
  COMMAND asc_coverage_build
  COMMAND common_coverage_build
  COMMAND cc_coverage_build
  COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_REPORT_DIR}
  COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --directory ${CMAKE_BINARY_DIR} --capture --output-file ${COVERAGE_REPORT_DIR}/${COVERAGE_TARGET}.info
  COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --remove ${COVERAGE_REPORT_DIR}/${COVERAGE_TARGET}.info ${COVERAGE_LCOV_EXCLUDES} '*/test/*' -o ${COVERAGE_REPORT_DIR}/${COVERAGE_TARGET}_filtered.info
  COMMAND ${GENHTML_PATH} ${COVERAGE_REPORT_DIR}/${COVERAGE_TARGET}_filtered.info -o ${COVERAGE_REPORT_DIR}
  COMMAND echo ""
  COMMAND echo "======================"
  COMMAND echo "Coverage report HTML : ${COVERAGE_REPORT_DIR}/index.html"
  COMMAND echo "======================"

  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running coverage..."
)
