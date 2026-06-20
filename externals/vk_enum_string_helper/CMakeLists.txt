cmake_minimum_required(VERSION 3.18)

message("")
message("  - Vulkan vk_enum_string_helper -")

project("vk_enum_string_helper" LANGUAGES C)

set(vk_enum_string_helper_TAG "vulkan-sdk-1.4.350.0" CACHE INTERNAL "Git tag")

add_library(${PROJECT_NAME} INTERFACE)
add_library(Vulkan::EnumStringHelper ALIAS ${PROJECT_NAME})

message(STATUS "Header-Only Library")

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} INTERFACE Vulkan::Headers)

include(CheckCCompilerFlag)

if (NOT MSVC)
    check_c_compiler_flag("-std=c17" COMPILER_SUPPORTS_C17)
else()
    check_c_compiler_flag("/std:c17" COMPILER_SUPPORTS_C17)
endif()

if (COMPILER_SUPPORTS_C++17)
    target_compile_features(${PROJECT_NAME} INTERFACE c_std_17)
    message(FATAL_ERROR "vk_enum_string_helper.h requires C17")
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_EXTENSIONS OFF)
