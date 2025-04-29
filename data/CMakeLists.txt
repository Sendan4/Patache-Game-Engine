add_custom_target(PatacheFiles
  COMMENT "Copying engine files")

if (MSVC)
  if (TARGET PatacheFiles)
		set_target_properties(PatacheFiles PROPERTIES FOLDER "Engine")
	endif()
endif()

# Copy Files
if (MSVC)
	add_custom_command(
        POST_BUILD
        TARGET PatacheFiles
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
            ${BIN_DIR}/${CMAKE_BUILD_TYPE}
    )

    add_custom_command(
        POST_BUILD
        TARGET PatacheFiles
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSES
            ${BIN_DIR}/${CMAKE_BUILD_TYPE}/LICENSES
    )

    # El Caso especial de Visual Studio. Sin esto, la bibloteca no podra encontrar el archivo YAML cuando se ejecute desde Visual Studio
    add_custom_command(
        POST_BUILD
        TARGET PatacheFiles
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
            ${CMAKE_BINARY_DIR}
    )
else ()
    # === GCC or Clang ===
    add_custom_command(
        POST_BUILD
        TARGET PatacheFiles
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
            ${BIN_DIR}
    )

    add_custom_command(
        POST_BUILD
        TARGET PatacheFiles
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSES
            ${BIN_DIR}/LICENSES
    )
endif ()
