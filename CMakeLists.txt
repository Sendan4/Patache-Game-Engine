message("")
message("  - Patata Engine -")

cmake_minimum_required(VERSION 3.18)

option(SHARED_BUILD "Build static or shared library" OFF)
option(LIBS_SHARED_BUILD "Build and link Shared dependencies" OFF)
option(USE_EXTERNAL_LIBS "Use libraries from external folder instead of system libraries" ON)
option(USE_GIT "Grab git information" ON)
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

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("PatataEngine"
  LANGUAGES CXX
  VERSION 1 
  HOMEPAGE_URL "https://gitlab.com/Sendan/patata-engine.git")

set (PATATA_ENGINE_VERSION_MAYOR 1)
set (PATATA_ENGINE_VERSION_MINOR 0)
set (PATATA_ENGINE_VERSION_PATCH 0)
set (PATATA_ENGINE_VERSION "1.0.0")

include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)
include(CheckTypeSize)

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	enable_language(C)
	include(CheckIncludeFile)
endif()

if (MINGW AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
	# Mingw specific function conversion in vulkan
	check_cxx_compiler_flag("-fpermissive" COMPILER_SUPPORTS_FPERMISSIVE)
endif()

if (NOT MSVC)
	check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_C++20) # Obligatory
	check_cxx_compiler_flag("-Wextra" COMPILER_SUPPORTS_WEXTRA)
	check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
	check_cxx_compiler_flag("-Wpedantic" COMPILER_SUPPORTS_WPEDANTIC)
	check_cxx_compiler_flag("-fno-exceptions" COMPILER_SUPPORTS_NO_EXCEPTIONS) # Obligatory
	check_cxx_compiler_flag("-fno-strict-aliasing" COMPILER_SUPPORTS_NO_STRICT_ALIASING)
	check_cxx_compiler_flag("-ffast-math" COMPILER_SUPPORTS_FAST_MATH) # Obligatory

  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    check_cxx_compiler_flag("-mmmx" COMPILER_SUPPORTS_MMX) # Obligatory
    check_cxx_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE4.2) # Obligatory
    check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX) # Obligatory
  endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		check_cxx_compiler_flag("-fno-rtti" COMPILER_SUPPORTS_NO_RTTI) # Obligatory

		if (NOT COMPILER_SUPPORTS_NO_RTTI) # Obligatory
    		message(FATAL_ERROR "The compiler does not support -fno-rtti")
		endif()
	endif()

	if (NOT COMPILER_SUPPORTS_C++20) # Obligatory
    	message(FATAL_ERROR "The compiler does not support the C++20 standard")
	endif()

	if (NOT COMPILER_SUPPORTS_NO_EXCEPTIONS) # Obligatory
    	message(FATAL_ERROR "Exceptions cannot be disabled in this compiler")
	endif()

	if (NOT COMPILER_SUPPORTS_FAST_MATH) # Obligatory
    	message(FATAL_ERROR "The compiler does not support -ffast-math")
	endif()

  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
	  if (NOT COMPILER_SUPPORTS_SSE4.2) # Obligatory
    	  message(FATAL_ERROR "The compiler does not support -msse4.2")
	  endif()

	  if (NOT COMPILER_SUPPORTS_MMX) # Obligatory
    	  message(FATAL_ERROR "The compiler does not support -mmmx")
	  endif()

    if (NOT COMPILER_SUPPORTS_AVX) # Obligatory
    	  message(FATAL_ERROR "The compiler does not support -mavx")
	  endif()
  endif()
else()
	check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_C++20) # Obligatory
	check_cxx_compiler_flag("/W4" COMPILER_SUPPORTS_W4)
	check_cxx_compiler_flag("/fp:fast" COMPILER_SUPPORTS_FLOATING_POINT_FAST) # Obligatory
	check_cxx_compiler_flag("/fp:except-" COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT) # Obligatory

	if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
		check_cxx_compiler_flag("/arch:SSE4.2" COMPILER_SUPPORTS_SSE4.2) # Obligatory
    check_cxx_compiler_flag("/arch:AVX" COMPILER_SUPPORTS_AVX) # Obligatory
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		check_cxx_compiler_flag("/GR-" COMPILER_SUPPORTS_NO_RTTI) # Obligatory

		if (NOT COMPILER_SUPPORTS_NO_RTTI) # Obligatory
    		message(FATAL_ERROR "The compiler does not support /GR-")
		endif()
	endif()

	if (NOT COMPILER_SUPPORTS_C++20) # Obligatory
    	message(FATAL_ERROR "The compiler does not support the C++20 standard")
	endif()

	if (NOT COMPILER_SUPPORTS_FLOATING_POINT_FAST) # Obligatory
    	message(FATAL_ERROR "The compiler does not support /fp:fast")
	endif()

	if (NOT COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT) # Obligatory
    	message(FATAL_ERROR "The compiler cannot disable excepctions for floating point")
	endif()

	if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
	  if (NOT COMPILER_SUPPORTS_SSE4.2) # Obligatory
    	  message(FATAL_ERROR "The compiler does not support /arch:SSE4.2")
	  endif()
    if (NOT COMPILER_SUPPORTS_AVX) # Obligatory
      message(FATAL_ERROR "The compiler does not support /arch:AVX")
	  endif()
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
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	include(CheckCXXSymbolExists)

	CHECK_INCLUDE_FILE_CXX("cstdio" HAVE_CSTDIO LANGUAGE CXX)
	if (NOT HAVE_CSTDIO)
		message(FATAL_ERROR "missing cstdio header")
	endif()

	check_cxx_symbol_exists("snprintf" "cstdio" HAVE_SNPRINTF)
	if (NOT HAVE_SNPRINTF)
		message(FATAL_ERROR "missing snprintf function")
	endif()

  if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    CHECK_INCLUDE_FILE_CXX("cstdlib" HAVE_CSTDLIB LANGUAGE CXX)
    if (NOT HAVE_CSTDLIB)
	    message(FATAL_ERROR "missing cstdlib header")
    endif()
  endif()
endif()

if (MSVC)
	set (CMAKE_REQUIRED_FLAGS "")
endif()

CHECK_INCLUDE_FILE_CXX("cstring" HAVE_CSTRING LANGUAGE CXX)
if (NOT HAVE_CSTRING)
	message(FATAL_ERROR "missing cstring header")
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

CHECK_INCLUDE_FILE_CXX("cstdint" HAVE_CSTDINT LANGUAGE CXX)
if (NOT HAVE_CSTDINT)
	message(FATAL_ERROR "missing cstdint header")
endif()

# Check Types
check_type_size(uint8_t UINT8_T LANGUAGE CXX)
if (NOT ${HAVE_UINT8_T})
	message(FATAL_ERROR "missing uint8_t type")
endif()

check_type_size(uint16_t UINT16_T LANGUAGE CXX)
if (NOT ${HAVE_UINT16_T})
	message(FATAL_ERROR "missing uint16_t type")
endif()

check_type_size(uint32_t UINT32_T LANGUAGE CXX)
if (NOT ${HAVE_UINT32_T})
	message(FATAL_ERROR "missing uint32_t type")
endif()

check_type_size(uint64_t UINT64_T LANGUAGE CXX)
if (NOT ${HAVE_UINT64_T})
	message(FATAL_ERROR "missing uint64_t type")
endif()

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
#include <cstdint>
int main() {
    #if !defined(UINT64_MAX)
    #error \"Missing UINT64_MAX\"
    #endif
    return 0;
}
" UINT64_MAX)
if (NOT UINT64_MAX)
	message(FATAL_ERROR "missing UINT64_MAX macro")
endif()

check_cxx_source_compiles("
#include <cstdint>
int main() {
    #if !defined(UINT32_MAX)
    #error \"Missing UINT32_MAX\"
    #endif
    return 0;
}
" UINT32_MAX)
if (NOT UINT32_MAX)
	message(FATAL_ERROR "missing UINT32_MAX macro")
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
" std_async)
if (NOT std_async)
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
endif()

if (NOT USE_EXTERNAL_LIBS)
  # === Package Mananger ===
	message("")
	message("  - fast_io -")
	add_subdirectory(externals/fast_io)

	message("")
	message("  - RapidYaml -")
	find_package(ryml)

  if (NOT ${ryml_FOUND})
		include(FetchContent)

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
    )
    
    set (RYML_DEV OFF CACHE INTERNAL "..." FORCE)
    set (RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
    set (RYML_INSTALL OFF CACHE INTERNAL "..." FORCE)
	  FetchContent_MakeAvailable(RapidYaml)
  endif()

	message("")
	message("  - Vulkan -")
	find_package(Vulkan)

	if (NOT ${Vulkan_VERSION} VERSION_GREATER_EQUAL 1.3.296)
		message(FATAL_ERROR "Vulkan >= 1.3.296")
	endif()

  if (NOT ${Vulkan_FOUND})
  	set (BUILD_TESTS OFF CACHE INTERNAL "..." FORCE)
    set (UPDATE_DEPS ON CACHE INTERNAL "..." FORCE)
    set (VULKAN_HEADERS_ENABLE_MODULE OFF CACHE INTERNAL "..." FORCE)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	    # Linux Wayland / Xorg
    	# Wayland Enable by default
    	set (BUILD_WSI_WAYLAND_SUPPORT ON CACHE INTERNAL "..." FORCE)

    	# Xorg Disable by default
    	if (NOT XORG_SUPPORT)
		    set (BUILD_WSI_XCB_SUPPORT OFF CACHE BOOL "..." FORCE)
    		set (BUILD_WSI_XLIB_SUPPORT OFF CACHE BOOL "..." FORCE)
    	else()
		    set (BUILD_WSI_XLIB_SUPPORT ON CACHE BOOL "..." FORCE)
    		set (BUILD_WSI_XCB_SUPPORT ON CACHE BOOL "..." FORCE)
    	endif()
    endif()

		include(FetchContent)

    FetchContent_Declare(
      Vulkan-Headers
      GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
      GIT_TAG "vulkan-sdk-1.4.309"
      BUILD_IN_SOURCE FALSE
      GIT_PROGRESS TRUE
      LOG_DOWNLOAD TRUE
      LOG_UPDATE TRUE
      LOG_CONFIGURE TRUE
      LOG_BUILD TRUE
      LOG_INSTALL TRUE
      GIT_SUBMODULES_RECURSE TRUE
    )

    FetchContent_Declare(
      Vulkan-Loader
      GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
      GIT_TAG "vulkan-sdk-1.4.309"
      BUILD_IN_SOURCE FALSE
      GIT_PROGRESS TRUE
      LOG_DOWNLOAD TRUE
      LOG_UPDATE TRUE
      LOG_CONFIGURE TRUE
      LOG_BUILD TRUE
      LOG_INSTALL TRUE
      GIT_SUBMODULES_RECURSE TRUE
    )

	  message("")
    message("  - Vulkan-Headers -")
    FetchContent_MakeAvailable(Vulkan-Headers)
	  message("")
    message("  - Vulkan-Loader -")
    FetchContent_MakeAvailable(Vulkan-Loader)
  endif()

	message("")
  message("  - SDL -")
  find_package(SDL2)

  if (NOT ${SDL2_FOUND} AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    include(FetchContent)

    FetchContent_Declare(
      SDL2
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG "release-2.30.8"
      BUILD_IN_SOURCE FALSE
      GIT_PROGRESS TRUE
      LOG_DOWNLOAD TRUE
      LOG_UPDATE TRUE
      LOG_CONFIGURE TRUE
      LOG_BUILD TRUE
      LOG_INSTALL TRUE
      GIT_SUBMODULES_RECURSE TRUE
    )

    if (NOT LIBS_SHARED_BUILD)
	    set(SDL_SHARED OFF CACHE INTERNAL "..." FORCE)
	    set(SDL_STATIC ON CACHE INTERNAL "..." FORCE)
	    if (SHARED_BUILD)
		    set(SDL_STATIC_PIC ON CACHE INTERNAL "..." FORCE)
	    endif()
    else ()
	    set(SDL_SHARED ON CACHE INTERNAL "..." FORCE)
	    set(SDL_STATIC OFF CACHE INTERNAL "..." FORCE)
    endif ()

    set(SDL_UNUSED_SUBSYSTEMS
      File
      Locale
	    Power
	    Render
	    Sensor
	    Timers
	    Atomic
	    Audio)
    foreach(_SUB ${SDL_UNUSED_SUBSYSTEMS})
	    string(TOUPPER ${_SUB} _OPT)
	    set(SDL_${_OPT} OFF CACHE INTERNAL "..." FORCE)
    endforeach()

    set (SDL_TEST OFF CACHE INTERNAL "..." FORCE)
    set (SDL_DIRECTX OFF CACHE INTERNAL "..." FORCE)
    set (SDL_RENDER_D3D OFF CACHE INTERNAL "..." FORCE)
    set (SDL_OPENGL OFF CACHE INTERNAL "..." FORCE)
    set (SDL_OPENGLES OFF CACHE INTERNAL "..." FORCE)
    set (SDL_RENDER OFF CACHE INTERNAL "..." FORCE)
    set (SDL_SNDIO OFF CACHE INTERNAL "..." FORCE)
    set (SDL_SNDIO_SHARED OFF CACHE INTERNAL "..." FORCE)
    set (SDL_PULSEAUDIO OFF CACHE INTERNAL "..." FORCE)
    set (SDL_PULSEAUDIO_SHARED OFF CACHE INTERNAL "..." FORCE)
    set (SDL_PULSEAUDIO_SHARED OFF CACHE INTERNAL "..." FORCE)
    set (SDL_PIPEWIRE OFF CACHE INTERNAL "..." FORCE)
    set (SDL_PIPEWIRE_SHARED OFF CACHE INTERNAL "..." FORCE)
    set (SDL_WASAPI OFF CACHE INTERNAL "..." FORCE)
    set (SDL_LIBSAMPLERATE OFF CACHE INTERNAL "..." FORCE)
    set (SDL_LIBSAMPLERATE_SHARED OFF CACHE INTERNAL "..." FORCE)

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	    if (NOT XORG_SUPPORT)
		    set (SDL_X11 OFF CACHE INTERNAL "..." FORCE)
		    set (SDL_X11_SHARED OFF CACHE INTERNAL "..." FORCE)
	    endif ()
    endif ()

    FetchContent_MakeAvailable(SDL2)
  endif()

  message("")
  message(" - FreeType -")
  find_package(Freetype)

  if (NOT ${Freetype_FOUND} AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    include(FetchContent)

    FetchContent_Declare(
      Freetype
      GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
      GIT_TAG "VER-2-13-3"
      BUILD_IN_SOURCE FALSE
      GIT_PROGRESS TRUE
      LOG_DOWNLOAD TRUE
      LOG_UPDATE TRUE
      LOG_CONFIGURE TRUE
      LOG_BUILD TRUE
      LOG_INSTALL TRUE
      GIT_SUBMODULES_RECURSE TRUE
      CMAKE_ARGS
        "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        -DRUNTIME_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    )

    set (FT_DISABLE_ZLIB ON CACHE INTERNAL "..." FORCE)
    set (FT_DISABLE_BZIP2 ON CACHE INTERNAL "..." FORCE)
    set (FT_DISABLE_PNG ON CACHE INTERNAL "..." FORCE)
    set (FT_DISABLE_HARFBUZZ ON CACHE INTERNAL "..." FORCE)
    set (FT_DISABLE_BROTLI ON CACHE INTERNAL "..." FORCE)

    FetchContent_MakeAvailable(Freetype)
  endif()

	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	   add_subdirectory(externals/imgui)
	endif()
else()
	# === using submodules ===
	add_subdirectory(externals)
endif()

# === Sources ===
add_subdirectory(src)

add_subdirectory(data)
add_subdirectory(data/assets)

target_compile_definitions(${PROJECT_NAME} PUBLIC SDL_MAIN_HANDLED)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
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
message(STATUS "USE_BIN_DIR : ${USE_BIN_DIR}")
if (DEFINED USE_VVL)
	message(STATUS "USE_VVL : ${USE_VVL}")
endif()
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
if (SHARED_BUILD)
	message(STATUS "Library Type : Shared")
else()
	message(STATUS "Library Type : Static")
endif()
if (USE_GIT AND GIT)
	message(STATUS "Git Branch : ${PATATA_GIT_BRANCH}")
	message(STATUS "Git Commit : ${PATATA_GIT_HASH_LONG}")
	message(STATUS "Version : ${PATATA_ENGINE_VERSION}")
endif()
message("")
