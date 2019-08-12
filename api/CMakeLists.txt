# CMake api namespace e.g. api::<some_service_lib>
set(API_NAMESPACE_NAME api)

add_subdirectory(machine_state_client)
add_subdirectory(application_state_client)
add_subdirectory(execution_management_api)

add_custom_target(gen
  COMMENT
    "Generate all Cap’n Proto files"
)

# Encapsulate all generating targets
add_dependencies(gen execution_management_api_gen)