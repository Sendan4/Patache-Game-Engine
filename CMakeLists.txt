message("")
message("  - Patache Engine -")

cmake_minimum_required(VERSION 3.18)

option(BUILD_SHARED_LIBS "Build static or shared library" OFF)
option(USE_GIT "Grab git information" ON)
option(BUILD_EXAMPLES "build program examples of the engine" OFF)
option(USE_BIN_DIR "use the bin directory as output" ON)
set(PATACHE_USE_BIN_DIR ${USE_BIN_DIR} PARENT_SCOPE)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(DEFAULT_USE_LTO_OPT ON)
else()
    set(DEFAULT_USE_LTO_OPT OFF)
endif()
    
option(USE_LTO "Use Link Time Optimization" ${DEFAULT_USE_LTO_OPT})

if (NOT MSVC)
    option(FORMAT_IN_BUILD_TIME "use clang-format to format in build time" OFF)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    option(USE_VVL "VVL is used to develop and test functions in the raccoon renderer" OFF)
endif ()

if (MSVC)
    set(CMAKE_SKIP_INSTALL_RULES YES)
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("PatacheEngine"
    LANGUAGES CXX
    VERSION 1 
    HOMEPAGE_URL "https://codeberg.org/PatacheEngine/Patache.git")

if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON CACHE INTERNAL "..." FORCE)
endif()

set (PATACHE_ENGINE_VERSION_MAYOR 0)
set (PATACHE_ENGINE_VERSION_MINOR 0)
set (PATACHE_ENGINE_VERSION_PATCH 0)
set (PATACHE_ENGINE_VERSION "0.0.0")

include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)
include(CheckTypeSize)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release" AND USE_LTO)
    include(CheckIPOSupported)
    
    check_ipo_supported(
  	RESULT COMPILER_SUPPORT_LTO
  	LANGUAGES CXX
    )
    
    if (NOT COMPILER_SUPPORT_LTO)
        set(USE_LTO OFF)
        message(STATUS "${CMAKE_CXX_COMPILER_ID} no support LTO")
    endif()
    
    # LTO dont work well with clang in static builds
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND NOT BUILD_SHARED_LIBS)
        set(USE_LTO OFF) 
    endif()
endif()


if (WIN32 OR ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    enable_language(C)
    include(CheckIncludeFile)
endif()

if (MINGW AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
    # Mingw specific function conversion in vulkan
    check_cxx_compiler_flag("-fpermissive" COMPILER_SUPPORTS_FPERMISSIVE)
endif()

if (NOT MSVC)
    # GCC
    check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_C++20) # Obligatory
    check_cxx_compiler_flag("-Wextra" COMPILER_SUPPORTS_WEXTRA)
    check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
    check_cxx_compiler_flag("-Wpedantic" COMPILER_SUPPORTS_WPEDANTIC)
    check_cxx_compiler_flag("-fno-exceptions" COMPILER_SUPPORTS_NO_EXCEPTIONS) # Obligatory
    check_cxx_compiler_flag("-fno-strict-aliasing" COMPILER_SUPPORTS_NO_STRICT_ALIASING)
    check_cxx_compiler_flag("-ffast-math" COMPILER_SUPPORTS_FAST_MATH) # Obligatory

    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        check_cxx_compiler_flag("-mmmx" COMPILER_SUPPORTS_MMX) # Obligatory
        check_cxx_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE4.2) # Obligatory
        check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX) # Obligatory
    endif()

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
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
    # MSVC
    check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_C++20) # Obligatory
    check_cxx_compiler_flag("/W4" COMPILER_SUPPORTS_W4)
    check_cxx_compiler_flag("/fp:fast" COMPILER_SUPPORTS_FLOATING_POINT_FAST) # Obligatory
    check_cxx_compiler_flag("/fp:except-" COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT) # Obligatory

    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        check_cxx_compiler_flag("/arch:SSE4.2" COMPILER_SUPPORTS_SSE4.2) # Obligatory
        check_cxx_compiler_flag("/arch:AVX" COMPILER_SUPPORTS_AVX) # Obligatory
    endif()

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
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

if (WIN32 OR ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)

    if (NOT HAVE_WINDOWS_H)
        message(FATAL_ERROR "missing windows.h")
    endif()
endif()

# Check Headers
include(CheckCXXSymbolExists)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        CHECK_INCLUDE_FILE_CXX("cxxabi.h" HAVE_CXXABI_H LANGUAGE CXX)
        
        if (NOT HAVE_CXXABI_H)
            message(FATAL_ERROR "include files missing")
        endif()
    endif()

    if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
        CHECK_INCLUDE_FILE_CXX("cstdlib" HAVE_CSTDLIB LANGUAGE CXX)

        if (NOT HAVE_CSTDLIB)
            message(FATAL_ERROR "missing cstdlib header")
        endif()
    endif()
endif()

CHECK_INCLUDE_FILE_CXX("cstdio" HAVE_CSTDIO LANGUAGE CXX)
if (NOT HAVE_CSTDIO)
    message(FATAL_ERROR "missing cstdio header")
endif()

check_cxx_symbol_exists("snprintf" "cstdio" HAVE_SNPRINTF)
if (NOT HAVE_SNPRINTF)
    message(FATAL_ERROR "missing snprintf function")
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
    include(CheckCXXSymbolExists)

    check_cxx_symbol_exists(setenv cstdlib HAVE_SETENV)

    if (NOT HAVE_SETENV)
        message(FATAL_ERROR "missing setenv function")
    endif()

    check_cxx_symbol_exists(getenv cstdlib HAVE_GETENV)

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

CHECK_INCLUDE_FILE_CXX("functional" HAVE_FUNCTIONAL LANGUAGE CXX)
if (NOT HAVE_FUNCTIONAL)
    message(FATAL_ERROR "missing functional header")
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" OR
    "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(CMAKE_REQUIRED_FLAGS_BACKUP ${CMAKE_REQUIRED_FLAGS})

    # Check std::async
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        string(APPEND CMAKE_REQUIRED_FLAGS "-std=c++20")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
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

    set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_BACKUP})
endif()

find_program(GIT NAMES git)

if (USE_GIT AND GIT)
    message (STATUS "Git Found : ${GIT}")

    # Git Macros
    execute_process(
        COMMAND git branch --show-current HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
        COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH_LONG
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
        COMMAND git diff HEAD --shortstat
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE PATACHE_GIT_IS_DIRTY
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
        COMMAND git diff HEAD --shortstat --staged
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE PATACHE_GIT_COMMIT_IS_IN_STAGE
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # The commit is Dirty?
    if (NOT "${PATACHE_GIT_IS_DIRTY}" STREQUAL "")
        set (GIT_DIRTY 1)
    else()
        set (GIT_DIRTY 0)
    endif()

    # The Changes are in the stage?
    if (NOT "${PATACHE_GIT_COMMIT_IS_IN_STAGE}" STREQUAL "")
        set (GIT_STAGE 1)
    else()
        set (GIT_STAGE 0)
    endif()
endif()

# Search Generator Version
if ("${CMAKE_GENERATOR}" STREQUAL "Ninja" OR "${CMAKE_GENERATOR}" STREQUAL "Eclipse CDT4 - Ninja")
    execute_process(
        COMMAND ninja --version
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE CMAKE_GENERATOR_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif ("${CMAKE_GENERATOR}" STREQUAL "Borland Makefiles" OR
        "${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles" OR
        "${CMAKE_GENERATOR}" STREQUAL "MSYS Makefiles" OR
        "${CMAKE_GENERATOR}" STREQUAL "MinGW Makefiles" OR
        "${CMAKE_GENERATOR}" STREQUAL "Eclipse CDT4 - MinGW Makefiles" OR
        "${CMAKE_GENERATOR}" STREQUAL "Eclipse CDT4 - Unix Makefiles" OR
        "${CMAKE_GENERATOR}" STREQUAL "KDevelop3 - Unix Makefiles")
    execute_process(
        COMMAND make --version
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE CMAKE_GENERATOR_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (NOT ${CMAKE_GENERATOR_VERSION} STREQUAL "")
        string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]"
            CMAKE_GENERATOR_VERSION
            ${CMAKE_GENERATOR_VERSION})
    endif()
else()
    set (CMAKE_GENERATOR_VERSION "")
endif()

get_filename_component(compiler_program ${CMAKE_CXX_COMPILER} NAME)

# shared library
if (BUILD_SHARED_LIBS)
    add_compile_definitions(SHARED_LIBRARY_EXPORT_DEFINE="1")
else ()
    add_compile_definitions(SHARED_LIBRARY_EXPORT_DEFINE="0")
endif()

if (USE_BIN_DIR)
    get_filename_component(BUILDDIR_NAME ${CMAKE_BINARY_DIR} NAME)

    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/${BUILDDIR_NAME})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILDDIR_NAME})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILDDIR_NAME})
    set(PATACHE_OUTPUT_DIR ${BIN_DIR} PARENT_SCOPE)
else()
    set(BIN_DIR ${CMAKE_BINARY_DIR})
endif()

add_subdirectory(externals)

# === Sources ===
add_subdirectory(src)

if(${BUILD_EXAMPLES})
    add_subdirectory(examples/Empty)
    add_subdirectory(examples/Triangle)
    add_subdirectory(examples/MultipleTriangles)
endif()

add_subdirectory(data)

if (NOT MSVC)
    include(cmake/ClangFormat.cmake)
endif()

if (BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE "Shared")
else()
    set(LIBRARY_TYPE "Static")
endif()

message("")
message(" - Patache Engine Configuration -")
message(STATUS "BUILD_SHARED_LIBS : ${BUILD_SHARED_LIBS} (${LIBRARY_TYPE})")
if (DEFINED USE_LTO)
    message(STATUS "LTO (Link Time Optimization) : ${USE_LTO}")
endif()
message(STATUS "USE_BIN_DIR : ${USE_BIN_DIR}")
if (DEFINED USE_VVL)
    message(STATUS "USE_VVL : ${USE_VVL}")
endif()
if (DEFINED FORMAT_IN_BUILD_TIME)
    message(STATUS "FORMAT_IN_BUILD_TIME : ${FORMAT_IN_BUILD_TIME}")
endif()
message(STATUS "BUILD_EXAMPLES : ${BUILD_EXAMPLES}")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
message(STATUS "USE_GIT : ${USE_GIT}")
if (USE_GIT AND GIT)
    message(STATUS "Git Branch : ${GIT_BRANCH}")
    message(STATUS "Git Commit : ${GIT_HASH_LONG}")
    message(STATUS "Version : ${PATACHE_ENGINE_VERSION}")
endif()
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    message(STATUS "CMAKE_DISABLE_PRECOMPILE_HEADERS has been enabled")
endif()
message("")
