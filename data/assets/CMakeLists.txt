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
		if (MSVC)
			file(COPY ${ICON_PATH}
				DESTINATION ${BIN_DIR}/${CMAKE_BUILD_TYPE})

			if (NOT ${GAME_NAME} STREQUAL "OFF")
				file(RENAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/icon.bmp"
					"${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.bmp")
			endif()
		else()
			file(COPY ${ICON_PATH}
				DESTINATION ${BIN_DIR})

			if (NOT ${GAME_NAME} STREQUAL "OFF")
				file(RENAME "${BIN_DIR}/icon.bmp"
					"${BIN_DIR}/${GAME_NAME}.bmp")
			else()
				file(RENAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icon.bmp"
					"${BIN_DIR}/${ICON_NAME}.bmp")
			endif()
		endif()
	else()
		if (MSVC)
			file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_ICON}
				DESTINATION ${BIN_DIR}/${CMAKE_BUILD_TYPE})

			if (NOT ${GAME_NAME} STREQUAL "OFF")
				file(RENAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${DEFAULT_ICON}
					"${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.bmp")
			endif()
		else()
			file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_ICON}
				DESTINATION ${BIN_DIR})

			if (NOT ${GAME_NAME} STREQUAL "OFF")
				file(RENAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ICON_NAME}
					"${BIN_DIR}/${GAME_NAME}.bmp")
			endif()
		endif()
	endif()
endif()
