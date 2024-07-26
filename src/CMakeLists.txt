file(GLOB_RECURSE Include "${CMAKE_CURRENT_SOURCE_DIR}/../include/PatataEngine/*.hpp")

if (DEFINED SHARED_BUILD AND SHARED_BUILD)
	add_library(${PROJECT_NAME} SHARED)
	add_library(Patata::Engine-Shared ALIAS ${PROJECT_NAME})
endif()

if (DEFINED SHARED_BUILD AND NOT SHARED_BUILD)
	add_library(${PROJECT_NAME} STATIC)
	add_library(Patata::Engine-Static ALIAS ${PROJECT_NAME})
endif()

target_sources (${PROJECT_NAME} PRIVATE
	"Core/PatataEngine.cpp"
	"Core/Input.cpp"
	"Core/Render.cpp"
	"Window/Window.cpp"
	"RaccoonRenderer/RaccoonRenderer.cpp"
	"RaccoonRenderer/Vulkan/Setup_Vulkan_Renderer.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Instance.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Select_Device.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Logical_And_Queue.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_SwapChain.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_DepthBuffer.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Render.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_PipeLine.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_RenderPass.cpp"
	"Log/Messages.cpp"
	"Log/YamlErrors.cpp")

if (MSVC)
	# List Headers Files in Visual Studio
	target_sources(${PROJECT_NAME} PRIVATE ${Include})
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows" OR WIN32)
	enable_language(RC)

	target_sources (${PROJECT_NAME} PRIVATE
		"Log/Win/Start_Patata_Log_Info_Win.cpp"
		"Log/Win/WindowLogWin.cpp"
		"Log/Win/Setup_Vulkan_Log_Win.cpp"
		"Log/Win/Vulkan_Info_Win.cpp"
		"PatataEngine.rc")
else()
	target_sources (${PROJECT_NAME} PRIVATE
		"Log/Start_Patata_Log_Info.cpp"
		"Log/WindowLog.cpp"
		"Log/Setup_Vulkan_Log.cpp"
		"Log/Vulkan_Info.cpp")
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_sources(${PROJECT_NAME} PRIVATE
		"ImGUI/ImGUISetupBackend.cpp"
		"ImGUI/ImGUIFrame.cpp"
		"ImGUI/DrawDebugUI.cpp")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}../include/")
target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/Core/"
	"${CMAKE_CURRENT_SOURCE_DIR}/ImGUI/"
	"${CMAKE_CURRENT_SOURCE_DIR}/Log/"
	"${CMAKE_CURRENT_SOURCE_DIR}/RaccoonRenderer/"
	"${CMAKE_CURRENT_SOURCE_DIR}/Window/")

# Install With CPack
install(FILES ${Include} DESTINATION "include/PatataEngine/")
install(FILES ${Include_graphics} DESTINATION "include/PatataEngine/Graphics")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	install(TARGETS Imgui
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

if (USE_EXTERNAL_LIBS)
	install(TARGETS yaml-cpp
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

if (LIBS_SHARED_BUILD AND USE_EXTERNAL_LIBS)
	install(TARGETS SDL2main SDL2
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

if (COMPILER_SUPPORTS_CXX20)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE "-std=c++20")
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_compile_options(${PROJECT_NAME} PRIVATE "/std:c++20")
	endif()
endif ()

if (COMPILER_SUPPORTS_W4)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
	endif()
endif ()

if (COMPILER_SUPPORTS_WEXTRA)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
	endif()
endif ()

# Macros
if (XORG_SUPPORT)
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_LINUX_XORG_SUPPORT=1)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Release")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Debug")
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Debug Optimized")
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Minimal Size Release")
endif()

if (NOT "${CMAKE_GENERATOR_VERSION}" STREQUAL "")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_SYSTEM_GENERATOR_VERSION="${CMAKE_GENERATOR_VERSION}")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
	PATATA_BUILD_SYSTEM="CMake"
	PATATA_BUILD_SYSTEM_VERSION="${CMAKE_VERSION}"
	PATATA_BUILD_SYSTEM_GENERATOR="${CMAKE_GENERATOR}"
	PATATA_ENGINE_VERSION="${PATATA_ENGINE_VERSION}"
	PATATA_ENGINE_VERSION_MAYOR=${PATATA_ENGINE_VERSION_MAYOR}
	PATATA_ENGINE_VERSION_MINOR=${PATATA_ENGINE_VERSION_MINOR}
	PATATA_ENGINE_VERSION_PATCH=${PATATA_ENGINE_VERSION_PATCH}
	PATATA_ENGINE_NAME="Patata Engine"
	PATATA_COMPILER_PROGRAM="${compiler_program}"
	PATATA_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
	PATATA_ARCH="${CMAKE_SYSTEM_PROCESSOR}"
	PATATA_GIT_BRANCH="${PATATA_GIT_BRANCH}"
	PATATA_GIT_HASH_LONG="${PATATA_GIT_HASH_LONG}"
	PATATA_GIT_HASH_SHORT="${PATATA_GIT_HASH_SHORT}"
	PATATA_GIT_WORK_DIR_IS_CLEAN="${PATATA_GIT_WORK_DIR_IS_CLEAN}"
	PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL=${PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL}
	PATATA_GIT_WORK_DIR_IS_STAGED="${PATATA_GIT_WORK_DIR_IS_STAGED}"
	PATATA_GIT_WORK_DIR_IS_STAGED_BOOL=${PATATA_GIT_WORK_DIR_IS_STAGED_BOOL}
	PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT="${Fast_io_COMMIT_SHORT_HASH}")

if (USE_EXTERNAL_LIBS)
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_YAML_CPP_VERSION="${PATATA_YAML-CPP_VERSION}")

	if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR MSVC)
		# Cannot compile Vulkan in Mingw
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			PATATA_VULKAN_LOADER_VERSION="${PATATA_VULKAN_LOADER_VERSION}"
			PATATA_VULKAN_VALIDATION_LAYERS_VERSION="${PATATA_VULKAN_VALIDATION_LAYERS_VERSION}"
			PATATA_VULKAN_HEADERS_VERSION="${PATATA_VULKAN_HEADERS_VERSION}")
	endif()
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_IMGUI_VERSION="${Imgui_VERSION}")
endif()

if (NOT ${GAME_NAME} STREQUAL "OFF")
	target_compile_definitions(${PROJECT_NAME} PUBLIC PATATA_GAME_NAME="${GAME_NAME}")
endif()

if (DEFINED GAME_VERSION_MAYOR AND DEFINED GAME_VERSION_MINOR AND GAME_VERSION_PATCH)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		PATATA_GAME_VERSION_MAYOR=${GAME_VERSION_MAYOR}
		PATATA_GAME_VERSION_MINOR=${GAME_VERSION_MINOR}
		PATATA_GAME_VERSION_PATCH=${GAME_VERSION_PATCH})
endif()

if (USE_VVL)
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_USE_VVL)
endif()

if (USE_EXTERNAL_LIBS)
	# Linux or MSVC
	if (MSVC OR LINUX OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
		target_link_libraries(${PROJECT_NAME} PUBLIC vulkan Vulkan::Headers)

		set_target_properties(asm_offset
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
			target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
		endif()
	else()
		# mingw
		message("")
		message("  - Vulkan SDK -")
		find_package(Vulkan REQUIRED)

		set (VULKANSDK_LIBS_PATH "${Vulkan_LIBRARIES}")
		string(REPLACE "/vulkan-1.lib" "" VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})
		string(STRIP VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})

		message(STATUS "Vulkan Libs Path : ${VULKANSDK_LIBS_PATH}")

		target_link_libraries(${PROJECT_NAME} PUBLIC
			-lvulkan-1
			-L${VULKANSDK_LIBS_PATH})

		target_include_directories(${PROJECT_NAME} PUBLIC "${Vulkan_INCLUDE_DIRS}")

		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
			set (VULKANSDK_VVL_PATH "${Vulkan_LIBRARIES}")
			string(REPLACE "/Lib/vulkan-1.lib" "" VULKANSDK_VVL_PATH ${VULKANSDK_VVL_PATH})
			string(STRIP VULKANSDK_VVL_PATH ${VULKANSDK_VVL_PATH})
			set (VULKANSDK_VVL_PATH "${VULKANSDK_VVL_PATH}/Bin")
			message(STATUS "Validation Layer Path : ${VULKANSDK_VVL_PATH}")

			target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_FIND_VVL_FROM_SDK PATATA_VVL_SDK_PATH="${VULKANSDK_VVL_PATH}")
		endif()
	endif()

	target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../externals/SDL/include")

	if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
		target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_WAYLAND)
		if (XORG_SUPPORT)
			target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_X11)
		endif()
	endif()

	if (LIBS_SHARED_BUILD)
		# Shared
		target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
	else()
		if (CMAKE_SYSTEM_NAME STREQUAL "Windows")	
			# Shared on windows
			target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
		else()
			# Static
			target_link_libraries(${PROJECT_NAME} PUBLIC SDL2-static)
		endif()

		if (NOT MSVC)
			if ((WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows") OR (LINUX OR CMAKE_SYSTEM_NAME STREQUAL "Linux"))
				target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
			endif()
		endif()
	endif()
else()
	# Package Mananger
	target_link_libraries(${PROJECT_NAME} PUBLIC
		${Vulkan_LIBRARIES}
		${SDL2_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC Fast_io yaml-cpp)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_link_libraries(${PROJECT_NAME} PUBLIC Imgui)
endif()

if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")

	if (USE_EXTERNAL_LIBS)
		#Yaml-cpp
		set_target_properties(yaml-cpp PROPERTIES FOLDER "Engine/Dependencies")
		set_target_properties(uninstall PROPERTIES FOLDER "Engine/Others/yamlcpp")
		# move CTest of yaml-cpp
		set_target_properties(Continuous PROPERTIES FOLDER "Engine/Others/yamlcpp")
		set_target_properties(Nightly PROPERTIES FOLDER "Engine/Others/yamlcpp")
		set_target_properties(Experimental PROPERTIES FOLDER "Engine/Others/yamlcpp")
		set_target_properties(NightlyMemoryCheck PROPERTIES FOLDER "Engine/Others/yamlcpp")

		#SDL2
		if (TARGET SDL2)
			set_target_properties(SDL2 PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		endif()
		set_target_properties(SDL2main PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		if (TARGET SDL2-static)
			set_target_properties(SDL2-static PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		endif()
		if (TARGET SDL2_test)
			set_target_properties(SDL2_test PROPERTIES FOLDER "Engine/Others/SDL2")
		endif()
		set_target_properties(sdl_headers_copy PROPERTIES FOLDER "Engine/Others/SDL2")

		#Vulkan
		set_target_properties(asm_offset PROPERTIES FOLDER "Engine/Others/Vulkan")
		set_target_properties(loader_specific_options PROPERTIES FOLDER "Engine/Others/Vulkan")
		set_target_properties(loader-unknown-chain PROPERTIES FOLDER "Engine/Others/Vulkan")
		set_target_properties(loader-opt PROPERTIES FOLDER "Engine/Others/Vulkan")
		set_target_properties(loader_asm_gen_files PROPERTIES FOLDER "Engine/Others/Vulkan")
		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR USE_VVL)
			set_target_properties(vvl PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
			set_target_properties(VkLayer_utils PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
			set_target_properties(vulkan PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		else()
			set_target_properties(vulkan PROPERTIES FOLDER "Engine/Dependencies")
		endif()
	endif()

	#Imgui
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
		set_target_properties(Imgui PROPERTIES FOLDER "Engine/Dependencies")
	endif()
endif()

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	VERSION "${PATATA_ENGINE_VERSION}")

