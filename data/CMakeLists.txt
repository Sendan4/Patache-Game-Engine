add_custom_target(Patata-Data)
if (MSVC)
	if (TARGET Patata-Data)
		set_target_properties(Patata-Data PROPERTIES FOLDER "Engine")
	endif()
endif()

# Copy Files
if (MSVC)
	add_custom_command(
        POST_BUILD
        TARGET Patata-Data
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml
            ${BIN_DIR}/${CMAKE_BUILD_TYPE}
        COMMENT "Copying the configuration file"
    )

    add_custom_command(
        POST_BUILD
        TARGET Patata-Data
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSES
            ${BIN_DIR}/${CMAKE_BUILD_TYPE}/LICENSES
        COMMENT "Copying the license directory"
    )

    # El Caso especial de Visual Studio. Sin esto, la bibloteca no podra encontrar el archivo YAML cuando se ejecute desde Visual Studio
    add_custom_command(
        POST_BUILD
        TARGET Patata-Data
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml
            ${CMAKE_BINARY_DIR}
        COMMENT "Copying the configuration file to a VS runtime path"
    )

    if (GAME_NAME)
        add_custom_command(
            POST_BUILD
            TARGET Patata-Data
            COMMAND ${CMAKE_COMMAND} -E rename
                ${BIN_DIR}/${CMAKE_BUILD_TYPE}/patata.yaml
                ${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.yaml
            COMMENT "Renaming the configuration file to ${GAME_NAME}"
        )
        
        # El Caso especial de Visual Studio. Sin esto, la bibloteca no podra encontrar el archivo YAML cuando se ejecute desde Visual Studio
        add_custom_command(
            POST_BUILD
            TARGET Patata-Data
            COMMAND ${CMAKE_COMMAND} -E rename
                ${CMAKE_BINARY_DIR}/patata.yaml
                ${CMAKE_BINARY_DIR}/${GAME_NAME}.yaml
            COMMENT "Renaming the configuration file in the VS runtime to ${GAME_NAME}"
        )
	endif()
else ()
    # === GCC or Clang ===
    add_custom_command(
        POST_BUILD
        TARGET Patata-Data
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml
            ${BIN_DIR}
        COMMENT "Copying the configuration file"
    )

    add_custom_command(
        POST_BUILD
        TARGET Patata-Data
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSES
            ${BIN_DIR}/LICENSES
        COMMENT "Copying the license directory"
    )

    if (GAME_NAME)
        add_custom_command(
            POST_BUILD
            TARGET Patata-Data
            COMMAND ${CMAKE_COMMAND} -E rename
                ${BIN_DIR}/patata.yaml
                ${BIN_DIR}/${GAME_NAME}.yaml
            COMMENT "Renaming the configuration file to ${GAME_NAME}"
        )
	endif()
endif ()
