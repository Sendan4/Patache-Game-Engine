if (DEFINED SHARED_BUILD AND SHARED_BUILD)
	add_library(${PROJECT_NAME} SHARED)
endif()

if (DEFINED SHARED_BUILD AND NOT SHARED_BUILD)
	add_library(${PROJECT_NAME} STATIC)
endif()

add_dependencies(${PROJECT_NAME} Patata-Data)

target_sources (${PROJECT_NAME} PRIVATE
	"Core/PatataEngine.cpp"
	"Core/Configuration.cpp"
	"Core/Input.cpp"
	"Core/ClearColor.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_SetupRenderer.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Instance.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Select_Device.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_LogicalDevice_And_GrahicsQueue.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_SwapChain.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_ImageView.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_CommandPoolAndBuffer.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_FrameBuffer.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_DepthBuffer.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_SincronizationPrimitives.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_Render.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_PipeLine.cpp"
	"RaccoonRenderer/Vulkan/Vulkan_RenderPass.cpp"
	"Log/Messages.cpp")

if (MSVC)
	# List Headers Files in Visual Studio
	file(GLOB_RECURSE Include "${CMAKE_CURRENT_SOURCE_DIR}/../include/PatataEngine/*.hpp")
	file(GLOB_RECURSE IncludeInternal "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

	target_sources(${PROJECT_NAME} PRIVATE ${Include} ${IncludeInternal})
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
		"ImGUI/SetupImgui.cpp")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/Core/"
	"${CMAKE_CURRENT_SOURCE_DIR}/ImGUI/"
	"${CMAKE_CURRENT_SOURCE_DIR}/Log/"
	"${CMAKE_CURRENT_SOURCE_DIR}/RaccoonRenderer/")

# Compiler Flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (COMPILER_SUPPORTS_C++20)
        target_compile_options(${PROJECT_NAME} PRIVATE "-std=c++20")
    endif ()
    if (COMPILER_SUPPORTS_WALL)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    endif ()
    if (COMPILER_SUPPORTS_WEXTRA)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
    endif ()
	if (COMPILER_SUPPORTS_WPEDANTIC)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wpedantic")
    endif ()
    if (COMPILER_SUPPORTS_NO_EXCEPTIONS)
        target_compile_options(${PROJECT_NAME} PRIVATE "-fno-exceptions")
    endif ()
    if (COMPILER_SUPPORTS_NO_STRICT_ALIASING)
        target_compile_options(${PROJECT_NAME} PRIVATE "-fno-strict-aliasing")
    endif ()
    if (COMPILER_SUPPORTS_NO_RTTI)
        target_compile_options(${PROJECT_NAME} PRIVATE "-fno-rtti")
    endif()
	if (COMPILER_SUPPORTS_FAST_MATH)
		target_compile_options(${PROJECT_NAME} PRIVATE "-ffast-math")
	endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (COMPILER_SUPPORTS_C++20)
		target_compile_options(${PROJECT_NAME} PRIVATE "/std:c++20")
	endif()
    if (COMPILER_SUPPORTS_W4)
        target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
    endif ()
    if (COMPILER_SUPPORTS_NO_RTTI)
        target_compile_options(${PROJECT_NAME} PRIVATE "/GR-")
    endif()
	if (COMPILER_SUPPORTS_FLOATING_POINT_FAST)
		target_compile_options(${PROJECT_NAME} PRIVATE "/fp:fast")
	endif()
	if (COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT)
		target_compile_options(${PROJECT_NAME} PRIVATE "/fp:except-")
	endif()
	if (COMPILER_SUPPORTS_NO_EXCEPCTION_HANDLING)
		target_compile_options(${PROJECT_NAME} PRIVATE "/EHs-c-")
	endif()

	message(STATUS "Disabling exceptions with _HAS_EXCEPTIONS=0 for STL")
	target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=0)
endif()

# Macros
if (XORG_SUPPORT)
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_LINUX_XORG_SUPPORT=1)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Release")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATATA_DEBUG=0)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Debug")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATATA_DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Debug Optimized")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATATA_DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_BUILD_TYPE="Minimal Size Release")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATATA_DEBUG=0)
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
	PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT="${Fast_io_COMMIT_SHORT_HASH}"
	VULKAN_HPP_NO_EXCEPTIONS
	VULKAN_HPP_NO_SMART_HANDLE)

if (MSVC OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=0)
endif()

# Git
if (USE_GIT AND GIT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PATATA_GIT_BRANCH="${PATATA_GIT_BRANCH}"
        PATATA_GIT_HASH_LONG="${PATATA_GIT_HASH_LONG}"
        PATATA_GIT_HASH_SHORT="${PATATA_GIT_HASH_SHORT}"
        PATATA_GIT_WORK_DIR_IS_CLEAN="${PATATA_GIT_WORK_DIR_IS_CLEAN}"
        PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL=${PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL}
        PATATA_GIT_WORK_DIR_IS_STAGED="${PATATA_GIT_WORK_DIR_IS_STAGED}"
        PATATA_GIT_WORK_DIR_IS_STAGED_BOOL=${PATATA_GIT_WORK_DIR_IS_STAGED_BOOL})
endif()

if (USE_EXTERNAL_LIBS)
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_RAPIDYAML_VERSION="${PATATA_RAPIDYAML_VERSION}")

	if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR MSVC)
		# Cannot compile Vulkan in Mingw
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			PATATA_VULKAN_LOADER_VERSION="${PATATA_VULKAN_LOADER_VERSION}"
			PATATA_VULKAN_HEADERS_VERSION="${PATATA_VULKAN_HEADERS_VERSION}")
	endif()
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_IMGUI_VERSION="${Imgui_VERSION}")
endif()

if (USE_VVL)
	target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_USE_VVL)
endif()

# Link Libraries
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	# Using VVL
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
		target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
	endif()

	# build Vulkan
	target_link_libraries(${PROJECT_NAME} PUBLIC vulkan Vulkan::Headers)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Link Vulkan from SDK
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

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_WAYLAND)
	if (XORG_SUPPORT)
		target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_X11)
	endif()
endif()

if (TARGET asm_offset)
	set_target_properties(asm_offset
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if (LIBS_SHARED_BUILD)
	# Shared
	target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2main SDL2::SDL2)
else()
	# Static
	target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2main SDL2-static)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC Fast_io)
target_link_libraries(${PROJECT_NAME} PRIVATE ryml::ryml freetype)
target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_link_libraries(${PROJECT_NAME} PUBLIC Imgui)
endif()

# MSVC Folders
if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")

	if (USE_EXTERNAL_LIBS)
		# RapidYaml
		if (TARGET ryml)
			set_target_properties(ryml PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
		endif()
		if (TARGET ryml-uninstall)
			set_target_properties(ryml-uninstall PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
		endif()
		if (TARGET c4core-amalgamate)
			set_target_properties(c4core-amalgamate PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
		endif()
		if (TARGET c4core)
			set_target_properties(c4core PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
		endif()

		#SDL2
		if (TARGET SDL2)
			set_target_properties(SDL2 PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		endif()
		if(TARGET SDL2-static)
			set_target_properties(SDL2-static PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		endif()
		set_target_properties(SDL2main PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		if (TARGET SDL2_test)
			set_target_properties(SDL2_test PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		endif()
		set_target_properties(sdl_headers_copy PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		if (TARGET uninstall)
			set_target_properties(uninstall PROPERTIES FOLDER "Engine/Dependencies/SDL2")
		endif()

		#Vulkan
		set_target_properties(asm_offset PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		set_target_properties(loader_specific_options PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		set_target_properties(loader-unknown-chain PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		if (TARGET loader-opt)
			set_target_properties(loader-opt PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		endif()
		if (TARGET Vulkan-Module)
			set_target_properties(Vulkan-Module PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		endif()
		if (TARGET gpu_av_spirv)
			set_target_properties(gpu_av_spirv PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		endif()
		set_target_properties(loader_asm_gen_files PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR USE_VVL)
			set_target_properties(vvl PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
			set_target_properties(VkLayer_utils PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
			set_target_properties(vulkan PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		else()
			set_target_properties(vulkan PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
		endif()

		# Freetype
		if (TARGET freetype)
			set_target_properties(freetype PROPERTIES FOLDER "Engine/Dependencies")
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
