# CMake api namespace e.g. api::<some_service_lib>
set(API_NAMESPACE_NAME api)

add_subdirectory(machine_state_client)
add_subdirectory(application_state_client)
add_subdirectory(component_client)
add_subdirectory(socket_handler)
add_subdirectory(base_client)
add_subdirectory(base_server)