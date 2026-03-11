cmake_minimum_required(VERSION 3.18)

message("")
message("  - fast_io -")

project("fast_io" LANGUAGES CXX)

set(Fast_io_BRANCH "master" CACHE INTERNAL "Git branch")

add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE FAST_IO_OPTIMIZE_TIME)

message(STATUS "Header-Only Library")

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fast_io/)
    target_include_directories(${PROJECT_NAME} INTERFACE "fast_io/include/")
else()
    FetchContent_Declare(
        fast_io
        GIT_REPOSITORY https://github.com/cppfastio/fast_io.git
        GIT_TAG ${Fast_io_BRANCH}
    )
    FetchContent_MakeAvailable(fast_io)
    
    target_include_directories(${PROJECT_NAME} INTERFACE "${fast_io_SOURCE_DIR}/include/")
endif()

include(CheckCXXCompilerFlag)

if (NOT MSVC)
    check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_C++20)
    check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_C++23)
else()
    check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_C++20)
    check_cxx_compiler_flag("/std:c++23" COMPILER_SUPPORTS_C++23)
endif()

if (COMPILER_SUPPORTS_C++23)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)
endif()

if (COMPILER_SUPPORTS_C++20)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
else()
    message(ERROR "fast_io requires C++20")
endif()

# Mingw in Msys2
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} INTERFACE "-lntdll")
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_EXTENSIONS OFF)
