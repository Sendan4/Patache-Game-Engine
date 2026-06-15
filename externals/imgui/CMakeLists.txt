cmake_minimum_required(VERSION 3.18)

project("imgui" LANGUAGES CXX C VERSION "1.92.6")

set(Imgui_VERSION "v1.92.7-docking" CACHE INTERNAL "Imgui version")

message("")
message("  - ImGui -")

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG ${Imgui_VERSION}
)
FetchContent_MakeAvailable(imgui)

include(CheckIncludeFile)

CHECK_INCLUDE_FILE("stdio.h" HAVE_STDIO_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("float.h" HAVE_FLOAT_H)
CHECK_INCLUDE_FILE("stdarg.h" HAVE_STDARG_H)
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("assert.h" HAVE_ASSERT_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("limits.h" HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("math.h" HAVE_MATH_H)

if (BUILD_SHARED_LIBS AND NOT MSVC)
    message(STATUS "Library Type : Shared")
    add_library(${PROJECT_NAME} SHARED)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE "-fPIC")
        message(STATUS "Build With Position Independent Code")
    endif()
else ()
    message(STATUS "Library Type : Static")
    add_library(${PROJECT_NAME} STATIC)
endif()

if (NOT MSVC OR MSYS AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
    enable_language(RC)
   target_sources(${PROJECT_NAME} PRIVATE "imgui.rc")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${imgui_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${imgui_SOURCE_DIR}/backends/")
target_include_directories(${PROJECT_NAME} PUBLIC "${imgui_SOURCE_DIR}/misc/freetype")

# Link Libraries
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR
    CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "DragonFly" OR
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    target_link_libraries(${PROJECT_NAME} PRIVATE vulkan)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"
    AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    target_link_libraries(${PROJECT_NAME} PRIVATE
        -lvulkan-1
        -L${VULKAN_RT_DLL_DIR})
endif()

# Move this binaries to a build dir instead of bin build
if (TARGET asm_offset)
    set_target_properties(asm_offset
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE freetype SDL3::SDL3 Vulkan::Headers)
target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS} ${SDL3_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    VERSION "${Imgui_VERSION}")

target_sources(${PROJECT_NAME} PRIVATE
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
    "${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp")

write_file("${imgui_SOURCE_DIR}/imconfig.h" "#define IMGUI_ENABLE_FREETYPE" APPEND)
