# Copy Files
if (MSVC)
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml" "${CMAKE_CURRENT_SOURCE_DIR}/LICENSES"
		DESTINATION ${BIN_DIR}/${CMAKE_BUILD_TYPE})

	if (NOT ${GAME_NAME} STREQUAL "OFF")
		file(RENAME "${BIN_DIR}/${CMAKE_BUILD_TYPE}/patata.yaml"
			"${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.yaml")

		install(FILES "${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.yaml"
			DESTINATION ".")
		# El Caso especial de Visual Studio. Sin esto, la bibloteca no podra encontrar el archivo YAML cuando se ejecute desde Visual Studio
		file(COPY "${BIN_DIR}/${CMAKE_BUILD_TYPE}/${GAME_NAME}.yaml"
			DESTINATION "${CMAKE_BINARY_DIR}/")
	else()
		install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml"
			DESTINATION ".")
		# El Caso especial de Visual Studio. Sin esto, la bibloteca no podra encontrar el archivo YAML cuando se ejecute desde Visual Studio
		file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml"
			DESTINATION ${CMAKE_BINARY_DIR})
	endif()
else ()
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml" "${CMAKE_CURRENT_SOURCE_DIR}/LICENSES"
		DESTINATION ${BIN_DIR})

	if (NOT ${GAME_NAME} STREQUAL "OFF")
		file(RENAME "${BIN_DIR}/patata.yaml"
			"${BIN_DIR}/${GAME_NAME}.yaml")

		install(FILES "${BIN_DIR}/${GAME_NAME}.yaml"
			DESTINATION ".")
	else()
		install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/patata.yaml"
			DESTINATION ".")
	endif()
endif ()


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/licenses-debug.txt" CACHE INTERNAL "...")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
	set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/licenses-release.txt" CACHE INTERNAL "...")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/LICENSES" DESTINATION ".")
