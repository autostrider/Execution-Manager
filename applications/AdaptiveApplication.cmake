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

        set(SERVICE_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${PROCESS}.service")
        set(SERVICE_OUTPUT "~/.config/systemd/user/${APP_NAME}_${PROCESS}.service")
        add_custom_command(
          COMMENT "Copy manifest of ${APP_NAME}"
          OUTPUT ${SERVICE_OUTPUT}
          DEPENDS ${SERVICE_INPUT}
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${SERVICE_INPUT}
          ${SERVICE_OUTPUT}
         )
        add_custom_target(${APP_NAME}_${PROCESS}_service ALL DEPENDS ${SERVICE_INPUT} ${SERVICE_OUTPUT})

        # WorkingDirectory=/home/roman/Execution-Manager/build/
        # ExecStart=/home/roman/Documents/Execution-Manager/build/bin/applications/AdaptiveApplication1/processes/proc1
    endforeach(PROCESS)
endfunction(add_adaptive_application)