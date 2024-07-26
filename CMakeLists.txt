cmake_minimum_required(VERSION 3.18)

message("")
message("  - Patata Engine -")

option(SHARED_BUILD "Build static or shared library" OFF)
option(LIBS_SHARED_BUILD "Build and link Shared dependencies" OFF)
option(USE_EXTERNAL_LIBS "Use libraries from external folder instead of system libraries" ON)
option(USE_GIT "Grab git information" ON)
option(USE_CUSTOM_ICON "..." OFF)
option(ENABLE_ICON "use icon in runtime" ON)
option(GAME_NAME "specify the name of the game" "")
option(USE_BIN_DIR "use the bin directory as output" ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	option(XORG_SUPPORT "Disable or enable Xorg suport | Xorg is almost obsolete" OFF)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	option(USE_VVL "VVL is used to develop and test functions in the raccoon renderer" OFF)
endif ()

if (MSVC)
	set(CMAKE_SKIP_INSTALL_RULES YES)
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set (CMAKE_FIND_PACKAGE_TARGETS_GLOBAL TRUE)

if (SHARED_BUILD)
	set (PATATA_SHARED_BUILD ON CACHE INTERNAL "...")
endif()

if (NOT SHARED_BUILD)
	set (PATATA_STATIC_BUILD ON CACHE INTERNAL "...")
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

project("PatataEngine" LANGUAGES CXX VERSION 1 HOMEPAGE_URL "https://gitlab.com/Sendan/patata-engine.git")

set (PATATA_ENGINE_VERSION_MAYOR 1)
set (PATATA_ENGINE_VERSION_MINOR 0)
set (PATATA_ENGINE_VERSION_PATCH 0)
set (PATATA_ENGINE_VERSION "1.0.0")

include(CheckIncludeFiles)
include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)
include(CheckTypeSize)

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	enable_language(C)
	include(CheckIncludeFile)
endif()

if (NOT MSVC)
	check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
	check_cxx_compiler_flag("-Wextra" COMPILER_SUPPORTS_WEXTRA)
	check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)

	if (NOT COMPILER_SUPPORTS_CXX20)
    	message(FATAL_ERROR "The compiler does not support the C++20 standard")
	endif()

else()
	check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_CXX20)
	check_cxx_compiler_flag("/W4" COMPILER_SUPPORTS_W4)

	if (NOT COMPILER_SUPPORTS_CXX20)
    	message(FATAL_ERROR "The compiler does not support the C++20 standard")
	endif()
endif()

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)
endif()

if (NOT HAVE_WINDOWS_H AND (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows"))
	message(FATAL_ERROR "missing windows.h")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	CHECK_INCLUDE_FILE_CXX("cxxabi.h" HAVE_CXXABI_H LANGUAGE CXX)

	if (NOT HAVE_CXXABI_H)
		message(FATAL_ERROR "include files missing")
	endif()
endif()

CHECK_INCLUDE_FILE_CXX("tuple" HAVE_TUPLE LANGUAGE CXX)
CHECK_INCLUDE_FILE_CXX("cstdlib" HAVE_CSTDLIB LANGUAGE CXX)
CHECK_INCLUDE_FILE_CXX("string" HAVE_STRING LANGUAGE CXX)
CHECK_INCLUDE_FILE_CXX("cstdint" HAVE_CSTDINT LANGUAGE CXX)

if (NOT HAVE_TUPLE OR NOT HAVE_CSTDLIB OR NOT HAVE_STRING OR NOT HAVE_CSTDINT)
	message(FATAL_ERROR "include files missing")
endif()

find_program(GIT NAMES git)

if (USE_GIT AND GIT)
	message (STATUS "Git Found : ${GIT}")
	# Git Macros
	execute_process(
		COMMAND git branch --show-current HEAD
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE PATATA_GIT_BRANCH
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND git rev-parse HEAD
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE PATATA_GIT_HASH_LONG
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND git rev-parse --short HEAD
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE PATATA_GIT_HASH_SHORT
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND git diff HEAD --shortstat
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE PATATA_GIT_WORK_DIR_TEXT
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND git diff HEAD --shortstat --staged
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE PATATA_GIT_COMMIT_STAGE_TEXT
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	# The commit is Dirty?
	if ("${PATATA_GIT_WORK_DIR_TEXT}" STREQUAL "")
		set (PATATA_GIT_WORK_DIR_IS_CLEAN "Clean")
		set (PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL 1)
	else()
		set (PATATA_GIT_WORK_DIR_IS_CLEAN "Dirty")
		set (PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL 0)
	endif()

	# The Changes are in the stage?
	if (NOT "${PATATA_GIT_COMMIT_STAGE_TEXT}" STREQUAL "")
		set (PATATA_GIT_WORK_DIR_IS_STAGED "Staged")
		set (PATATA_GIT_WORK_DIR_IS_STAGED_BOOL 1)
	else()
		set (PATATA_GIT_WORK_DIR_IS_STAGED "Not Staged")
		set (PATATA_GIT_WORK_DIR_IS_STAGED_BOOL 0)
	endif()

	if (USE_EXTERNAL_LIBS)
		execute_process(
			COMMAND git describe --tags --abbrev=0
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/yaml-cpp/"
			OUTPUT_VARIABLE PATATA_YAML-CPP_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE)

		execute_process(
			COMMAND git describe --tags --abbrev=0
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/Vulkan-Loader/"
			OUTPUT_VARIABLE PATATA_VULKAN_LOADER_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE)

		execute_process(
			COMMAND git describe --tags --abbrev=0
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/Vulkan-Headers/"
			OUTPUT_VARIABLE PATATA_VULKAN_HEADERS_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE)
		
		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
			execute_process(
				COMMAND git describe --tags --abbrev=0
				WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/Vulkan-ValidationLayers/"
				OUTPUT_VARIABLE PATATA_VULKAN_VALIDATION_LAYERS_VERSION
				OUTPUT_STRIP_TRAILING_WHITESPACE)
		endif()
	endif()
endif()

# Search Generator Version
if ("${CMAKE_GENERATOR}" STREQUAL "Ninja" OR "${CMAKE_GENERATOR}" STREQUAL "Eclipse CDT4 - Ninja")
	execute_process(
		COMMAND ninja --version
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE CMAKE_GENERATOR_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif ("${CMAKE_GENERATOR}" STREQUAL "Borland Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "MSYS Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "MinGW Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "Eclipse CDT4 - MinGW Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "Eclipse CDT4 - Unix Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "KDevelop3 - Unix Makefiles")
	execute_process(
		COMMAND make --version
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE CMAKE_GENERATOR_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]" CMAKE_GENERATOR_VERSION ${CMAKE_GENERATOR_VERSION})
else()
	set (CMAKE_GENERATOR_VERSION "")
endif()

get_filename_component(compiler_program ${CMAKE_CXX_COMPILER} NAME)

if (ENABLE_ICON)
	add_compile_definitions(USE_ICON=1)
endif()

# shared library
if (SHARED_BUILD)
	add_compile_definitions(SHARED_LIBRARY_EXPORT_DEFINE="1")
else ()
	add_compile_definitions(SHARED_LIBRARY_EXPORT_DEFINE="0")
endif()

if (USE_BIN_DIR)
	set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
else()
	set(BIN_DIR ${CMAKE_BINARY_DIR})
	message(${CMAKE_BINARY_DIR})
endif()

if (NOT USE_EXTERNAL_LIBS)
	# Package Mananger
	add_subdirectory(externals/fast_io)
	find_package(yaml-cpp REQUIRED)
	find_package(Vulkan REQUIRED)
	find_package(SDL2 REQUIRED)

	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
		add_subdirectory(externals/imgui)

		find_file(HAVE_IMGUI_VULKAN_H "imgui_impl_vulkan.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/externals/imgui/backends")
		find_file(HAVE_IMGUI_SDL2_H "imgui_impl_sdl2.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/externals/imgui/backends")

		if (NOT HAVE_IMGUI_VULKAN_H OR NOT HAVE_IMGUI_SDL2_H)
			message(FATAL_ERROR "Missing Headers")
		endif()
	endif()

	find_file(HAVE_VULKAN_HPP "vulkan/vulkan.hpp" PATHS ${Vulkan_INCLUDE_DIRS})
	find_file(HAVE_SDL_H "SDL.h" PATHS ${SDL2_INCLUDE_DIRS})
	find_file(HAVE_SDL_VULKAN_H "SDL_vulkan.h" PATHS ${SDL2_INCLUDE_DIRS})
	find_file(HAVE_SDL_OPENGL_H "SDL_opengl.h" PATHS ${SDL2_INCLUDE_DIRS})
	find_file(HAVE_YAML_CPP_H "yaml-cpp/yaml.h" PATHS ${Yaml-cpp_INCLUDE_DIRS})

	message("")
	message("  - Patata Engine -")

	if (NOT HAVE_SDL_H OR NOT HAVE_SDL_VULKAN_H OR NOT HAVE_SDL_OPENGL_H OR NOT HAVE_VULKAN_HPP OR NOT HAVE_YAML_CPP_H)
		message(FATAL_ERROR "Missing Headers")
	else()
		if (HAVE_VULKAN_HPP)
			message(STATUS "Looking for vulkan/vulkan.hpp - found")
		endif()

		if (HAVE_SDL_H)
			message(STATUS "Looking for SDL.h - found")
		endif()

		if (HAVE_SDL_VULKAN_H)
			message(STATUS "Looking for SDL_vulkan.h - found")
		endif()

		if (HAVE_SDL_OPENGL_H)
			message(STATUS "Looking for SDL_opengl.h - found")
		endif()

		if (HAVE_YAML_CPP_H)
			message(STATUS "Looking for yaml-cpp/yaml.h - found")
		endif()

		if (HAVE_IMGUI_VULKAN_H)
			message(STATUS "Looking for imgui_impl_vulkan.h - found")
		endif()

		if (HAVE_IMGUI_SDL2_H)
			message(STATUS "Looking for imgui_impl_sdl2.h - found")
		endif()

		if (HAVE_IMGUI_OPENGL3_H)
			message(STATUS "Looking for imgui_impl_opengl3.h - found")
		endif()
	endif()
else()
	# using submodules
	add_subdirectory(externals)
endif()

add_subdirectory(data)
add_subdirectory(data/assets)

# Library
include_directories(include/)
add_subdirectory(src)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
if (SHARED_BUILD)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-Shared")
else()
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-Static")
endif()

include(CPack)

message("")
message(" - Patata Engine Configuration -")
message(STATUS "SHARED_BUILD : ${SHARED_BUILD}")
message(STATUS "LIBS_SHARED_BUILD : ${LIBS_SHARED_BUILD}")
message(STATUS "USE_EXTERNAL_LIBS : ${USE_EXTERNAL_LIBS}")
if (DEFINED XORG_SUPPORT)
	message(STATUS "XORG_SUPPORT : ${XORG_SUPPORT}")
endif()
message(STATUS "USE_GIT : ${USE_GIT}")
message(STATUS "ENABLE_ICON : ${ENABLE_ICON}")
message(STATUS "USE_CUSTOM_ICON : ${USE_CUSTOM_ICON}")
message(STATUS "USE_BIN_DIR : ${USE_BIN_DIR}")
if (DEFINED USE_VVL)
	message(STATUS "USE_VVL : ${USE_VVL}")
endif()
if (USE_CUSTOM_ICON)
	message(STATUS "Custom Icon Path : ${ICON_PATH}")
endif()
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
if (SHARED_BUILD)
	message(STATUS "Library Type : Shared")
else()
	message(STATUS "Library Type : Static")
endif()
if (NOT ${GAME_NAME} STREQUAL "OFF")
	message(STATUS "Game Name : ${GAME_NAME}")
endif()
if (USE_GIT AND GIT)
	message(STATUS "Git Branch : ${PATATA_GIT_BRANCH}")
	message(STATUS "Git Commit : ${PATATA_GIT_HASH_LONG}")
	message(STATUS "Version : ${PATATA_ENGINE_VERSION}")
endif()
message("")
