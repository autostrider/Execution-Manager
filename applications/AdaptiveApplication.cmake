function(add_adaptive_application)
    cmake_parse_arguments(
        APP
        ""
        "NAME"
        "PROCESSES"
        ${ARGN}
    )
    if(NOT APP_NAME)
        message(FATAL_ERROR "You must provide a name of Adaptive Application")
    endif(NOT APP_NAME)

    list(LENGTH APP_PROCESSES PROCESSES_LENGTH)
    if(PROCESSES_LENGTH LESS 1)
      message(FATAL_ERROR "You must provide at least one process")
    endif(PROCESSES_LENGTH LESS 1)

    set(MANIFEST_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/manifest.json")
    set(MANIFEST_OUTPUT "${CMAKE_BINARY_DIR}/bin/applications/${APP_NAME}/manifest.json")
    add_custom_command(
      COMMENT "Copy manifest of ${APP_NAME}"
      OUTPUT ${MANIFEST_OUTPUT}
      DEPENDS ${MANIFEST_INPUT}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${MANIFEST_INPUT}
      ${MANIFEST_OUTPUT}
     )
    add_custom_target(${APP_NAME}_manifest ALL DEPENDS ${MANIFEST_INPUT} ${MANIFEST_OUTPUT})

    message("Adaptive application '${APP_NAME}' added with ${PROCESSES_LENGTH} processes:") 
    foreach(PROCESS ${APP_PROCESSES})
        message("@ ${PROCESS}")

        set_target_properties(${PROCESS} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/applications/${APP_NAME}/processes/"
        )

        add_dependencies(${PROCESS} ${APP_NAME}_manifest)
        
        set(SUDO_PATH "/etc/systemd/user/")
				set(ENV_PATH "$ENV{XDG_RUNTIME_DIR}/systemd/user/")
        set(SERVICE_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${PROCESS}.service")
        set(SERVICE_OUTPUT "${SUDO_PATH}${APP_NAME}_${PROCESS}.service")

        file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${PROCESS}.service files)
        string(APPEND files "\nWorkingDirectory=${CMAKE_BINARY_DIR}\nExecStart=${CMAKE_BINARY_DIR}/bin/applications/${APP_NAME}/processes/${PROCESS}")
				file(WRITE ${ENV_PATH}${APP_NAME}_${PROCESS}.service ${files})

    endforeach(PROCESS)
endfunction(add_adaptive_application)
