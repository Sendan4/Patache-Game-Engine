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

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	option(USE_VVL "VVL is used to develop and test functions in the raccoon renderer" OFF)
endif ()

if (MSVC)
	set(CMAKE_SKIP_INSTALL_RULES YES)
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

include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	enable_language(C)
	include(CheckIncludeFile)
endif()

if (NOT MSVC)
	check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_C++20) # Obligatory
	check_cxx_compiler_flag("-Wextra" COMPILER_SUPPORTS_WEXTRA)
	check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
    check_cxx_compiler_flag("-fno-exceptions" COMPILER_SUPPORTS_NO_EXCEPTIONS)
    check_cxx_compiler_flag("-fno-strict-aliasing" COMPILER_SUPPORTS_NO_STRICT_ALIASING)
    if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        check_cxx_compiler_flag("-fno-rtti" COMPILER_SUPPORTS_NO_RTTI)
    endif()

	if (NOT COMPILER_SUPPORTS_C++20) # Obligatory
    	message(FATAL_ERROR "The compiler does not support the C++20 standard")
	endif()
else()
	check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_C++20) # Obligatory
	check_cxx_compiler_flag("/W4" COMPILER_SUPPORTS_W4)
    if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        check_cxx_compiler_flag("/GR-" COMPILER_SUPPORTS_NO_RTTI)
    endif()

	if (NOT COMPILER_SUPPORTS_C++20) # Obligatory
    	message(FATAL_ERROR "The compiler does not support the C++20 standard")
	endif()
endif()

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)

    if (NOT HAVE_WINDOWS_H AND (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows"))
	   message(FATAL_ERROR "missing windows.h")
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	CHECK_INCLUDE_FILE_CXX("cxxabi.h" HAVE_CXXABI_H LANGUAGE CXX)

	if (NOT HAVE_CXXABI_H)
		message(FATAL_ERROR "include files missing")
	endif()
endif()

# Check Headers
CHECK_INCLUDE_FILE_CXX("string" HAVE_STRING LANGUAGE CXX)
if (NOT HAVE_STRING)
	message(FATAL_ERROR "missing string header")
endif()

check_cxx_source_compiles("
#include <string>
int main() {
    std::string Test;
    return 0;
}
" HAVE_STD::STRING)
if (NOT HAVE_STD::STRING)
	message(FATAL_ERROR "missing std::string type")
endif()

check_cxx_source_compiles("
#include <string>
int main() {
    std::string_view Test;
    return 0;
}
" HAVE_STD::STRING_VIEW)
if (NOT HAVE_STD::STRING_VIEW)
	message(FATAL_ERROR "missing std::string_view type")
endif()

CHECK_INCLUDE_FILE_CXX("cstdlib" HAVE_CSTDLIB LANGUAGE CXX)
if (NOT HAVE_CSTDLIB)
	message(FATAL_ERROR "missing cstdlib header")
endif()

# Check setenv y getenv
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    check_cxx_source_compiles(
        "#include <cstdlib>\nint main(void) { setenv(\"MY_VAR\", \"VALUE\", 1); return 0; }"
        HAVE_SETENV)

    if (NOT HAVE_SETENV)
        message(FATAL_ERROR "missing setenv function")
    endif()

    check_cxx_source_compiles(
        "#include <cstdlib>\nint main(void) { getenv(\"HOME\"); return 0; }"
        HAVE_GETENV)

    if (NOT HAVE_GETENV)
        message(FATAL_ERROR "missing getenv function")
    endif()
endif()

CHECK_INCLUDE_FILE_CXX("limits" HAVE_CSTDINT LANGUAGE CXX)
if (NOT HAVE_CSTDINT)
	message(FATAL_ERROR "missing limits header")
endif()

CHECK_INCLUDE_FILE_CXX("cstdint" HAVE_CSTDINT LANGUAGE CXX)
if (NOT HAVE_CSTDINT)
	message(FATAL_ERROR "missing cstdint header")
endif()

# Check Types
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <cstdint>
int main() {
    uint8_t test = 0;
    return 0;
}
" HAVE_UINT8_T)
if (NOT HAVE_UINT8_T)
	message(FATAL_ERROR "missing uint8_t type")
endif()

check_cxx_source_compiles("
#include <cstdint>
int main() {
    uint16_t test = 0;
    return 0;
}
" HAVE_UINT16_T)
if (NOT HAVE_UINT16_T)
	message(FATAL_ERROR "missing uint16_t type")
endif()

check_cxx_source_compiles("
#include <cstdint>
int main() {
    uint32_t test = 0;
    return 0;
}
" HAVE_UINT32_T)
if (NOT HAVE_UINT32_T)
	message(FATAL_ERROR "missing uint32_t type")
endif()

check_cxx_source_compiles("
#include <cstdint>
int main() {
    uint64_t test = 0;
    return 0;
}
" HAVE_UINT64_T)
if (NOT HAVE_UINT64_T)
	message(FATAL_ERROR "missing uint64_t type")
endif()

check_cxx_source_compiles("
#include <cstdint>
int main() {
    #if !defined(UINT64_MAX)
    #error \"Missing UINT64_MAX\"
    #endif
    return 0;
}
" HAVE_MACRO_UINT64_MAX)
if (NOT HAVE_MACRO_UINT64_MAX)
	message(FATAL_ERROR "missing UINT64_T macro")
endif()

# Check Headers
CHECK_INCLUDE_FILE_CXX("future" HAVE_FUTURE LANGUAGE CXX)
if (NOT HAVE_FUTURE)
	message(FATAL_ERROR "missing future header")
endif()


# Check std::async
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_REQUIRED_FLAGS "-std=c++20")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(APPEND CMAKE_REQUIRED_FLAGS "/std:c++20")
endif()
check_cxx_source_compiles("
#include <future>
#include <cstdint>
void test(void);
int main(void) {
    auto Test = std::async(std::launch::async, test);
    return 0;
}
void test(void) { uint8_t c = 0; c++;};
" HAVE_STD_ASYNC)
if (NOT HAVE_STD_ASYNC)
	message(FATAL_ERROR "missing std::async function")
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
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/rapidyaml/"
			OUTPUT_VARIABLE PATATA_RAPIDYAML_VERSION
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

	message("")
	message("  - RapidYaml -")
	find_package(ryml)
	if (NOT ${ryml_FOUND})
		include(FetchContent)

		message("")
		message("  - RapidYaml -")
        FetchContent_Declare(
           RapidYaml
           GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
           GIT_TAG "v0.7.2"
           BUILD_IN_SOURCE FALSE
           GIT_PROGRESS TRUE
           LOG_DOWNLOAD TRUE
           LOG_UPDATE TRUE
           LOG_CONFIGURE TRUE
           LOG_BUILD TRUE
           LOG_INSTALL TRUE
           GIT_SUBMODULES_RECURSE TRUE
           CMAKE_ARGS
               "-DRYML_DEV=OFF
               -DRYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=OFF
               -DRYML_INSTALL=OFF"
        )

		FetchContent_MakeAvailable(RapidYaml)
		message("")
	endif()

	find_package(Vulkan REQUIRED)
	if (NOT ${Vulkan_VERSION} VERSION_GREATER_EQUAL 1.3.296)
		message(FATAL_ERROR "Vulkan >= 1.3.296")
	endif()

	find_package(SDL2 REQUIRED)
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	   add_subdirectory(externals/imgui)
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

target_compile_definitions(${PROJECT_NAME} PUBLIC SDL_MAIN_HANDLED)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_definitions(${PROJECT_NAME} PUBLIC main=SDL_main)
endif()

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
