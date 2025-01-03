cmake_minimum_required(VERSION 3.18)

project("Fast_io" LANGUAGES CXX)

set(Fast_io_COMMIT_SHORT_HASH "788cb98" CACHE INTERNAL "Git Commit Short Hash")

add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE FAST_IO_OPTIMIZE_TIME)

message(STATUS "Header-Only Library")

target_include_directories(${PROJECT_NAME} INTERFACE "include/")

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

include(CheckCXXCompilerFlag)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_C++23)
    if (COMPILER_SUPPORTS_C++23)
        target_compile_options(${PROJECT_NAME} INTERFACE "-std=c++23")
    endif()
endif()

# Mingw in Msys2
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME} INTERFACE "-lntdll")
endif()
