# ${ICON_PATH} is the absolute path to a "icon.bmp"

add_custom_target(Patata-Assets)
if (MSVC)
	if (TARGET Patata-Assets)
		set_target_properties(Patata-Assets PROPERTIES FOLDER "Engine")
	endif()
endif()

set (DEFAULT_ICON "")
set (ICON_NAME "")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	set (DEFAULT_ICON "icon/icon-debug.bmp")
	set (ICON_NAME "icon-debug.bmp")
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
	set (DEFAULT_ICON "icon/icon-release.bmp")
	set (ICON_NAME "icon-release.bmp")
endif()

if (ENABLE_ICON)
	if (USE_CUSTOM_ICON)
        # Custom Icon
	    if (MSVC)
            # === MSVC ===
            # Copying icon image
            add_custom_command(
                POST_BUILD
                TARGET Patata-Assets
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${ICON_PATH}
                    ${BIN_DIR}/${CMAKE_BUILD_TYPE}
                COMMENT "Copying icon image"
            )

            # El Caso especial de Visual Studio.
            add_custom_command(
                POST_BUILD
                TARGET Patata-Assets
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${ICON_PATH}
                    ${CMAKE_BINARY_DIR}
                COMMENT "Copying icon image to the VS runtime path"
            )

            if (GAME_NAME)
                # Renaming the custom icon name to a game name
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${BIN_DIR}/${CMAKE_BUILD_TYPE}/icon.bmp
                        ${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.bmp
                    COMMENT "Renaming the icon to ${GAME_NAME}"
                )

                # El Caso especial de Visual Studio.
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${CMAKE_BINARY_DIR}/icon.bmp
                        ${CMAKE_BINARY_DIR}/${GAME_NAME}.bmp
                    COMMENT "Renaming icon image int the VS runtime path to ${GAME_NAME}"
                )
	        else()
                # if GAME_NAME is empty
                # Changing the custom icon name to a default name
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${BIN_DIR}/${CMAKE_BUILD_TYPE}/icon.bmp
                        ${BIN_DIR}/${CMAKE_BUILD_TYPE}/${ICON_NAME}
                    COMMENT "Renaming the icon to ${ICON_NAME}"
                )

                # El Caso especial de Visual Studio.
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${CMAKE_BINARY_DIR}/icon.bmp
                        ${CMAKE_BINARY_DIR}/${ICON_NAME}
                    COMMENT "Renaming icon image int the VS runtime path to ${ICON_NAME}"
                )
		    endif()
        else()
            # === GCC or Clang ===
            # Copying icon image
            add_custom_command(
                POST_BUILD
                TARGET Patata-Assets
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${ICON_PATH}
                    ${BIN_DIR}
                COMMENT "Copying icon image"
            )

            if (GAME_NAME)
                # Renaming the custom icon name to a game name
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${BIN_DIR}/icon.bmp
                        ${BIN_DIR}/${GAME_NAME}.bmp
                    COMMENT "Renaming the icon to ${GAME_NAME}"
                )
	        else()
                # if GAME_NAME is empty
                # Changing the custom icon name to a default name
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${BIN_DIR}/icon.bmp
                        ${BIN_DIR}/${ICON_NAME}
                    COMMENT "Renaming the icon to ${ICON_NAME}"
                )
		    endif()
        endif()
	else()
        # Default icon
		if (MSVC)
            # === MSVC ===
            # Copying icon image
            add_custom_command(
                POST_BUILD
                TARGET Patata-Assets
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_ICON}
                    ${BIN_DIR}/${CMAKE_BUILD_TYPE}
                COMMENT "Copying icon image"
            )

            # El Caso especial de Visual Studio.
            add_custom_command(
                POST_BUILD
                TARGET Patata-Assets
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_ICON}
                    ${CMAKE_BINARY_DIR}/${ICON_NAME}
                COMMENT "Copying icon image to the VS runtime path"
            )

            if (GAME_NAME)
                # Changing the icon name to a default name
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${BIN_DIR}/${CMAKE_BUILD_TYPE}/${DEFAULT_ICON}
                        ${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.bmp
                    COMMENT "Renaming the icon to ${GAME_NAME}"
                )

                # El Caso especial de Visual Studio.
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${CMAKE_BINARY_DIR}/${ICON_NAME}
                        ${CMAKE_BINARY_DIR}/${GAME_NAME}.bmp
                    COMMENT "Renaming icon image int the VS runtime path to ${GAME_NAME}"
                )
			endif()
		else()
            # === GCC or Clang ===
            # Copying icon image
            add_custom_command(
                POST_BUILD
                TARGET Patata-Assets
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_ICON}
                    ${BIN_DIR}
                COMMENT "Copying icon image"
             )

            if (GAME_NAME)
                # Changing the icon name to a default name
                add_custom_command(
                    POST_BUILD
                    TARGET Patata-Assets
                    COMMAND ${CMAKE_COMMAND} -E rename
                        ${BIN_DIR}/${ICON_NAME}
                        ${BIN_DIR}/${GAME_NAME}.bmp
                    COMMENT "Renaming the icon to ${GAME_NAME}"
                )
			endif()
		endif()
	endif()
endif()
