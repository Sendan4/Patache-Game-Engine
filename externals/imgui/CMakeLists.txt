cmake_minimum_required(VERSION 3.18)

project("Imgui" LANGUAGES CXX C VERSION 1.90.8)

set(Imgui_VERSION "v1.91.5-docking" CACHE INTERNAL "Imgui version")

message("")
message("  - Imgui -")

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

if (LIBS_SHARED_BUILD AND NOT MSVC)
	message(STATUS "Library Type : Shared")
	add_library(${PROJECT_NAME} SHARED)

    if (SHARED_BUILD AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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

target_include_directories(${PROJECT_NAME} PUBLIC ".")
target_include_directories(${PROJECT_NAME} PUBLIC "backends/")
target_include_directories(${PROJECT_NAME} PUBLIC "misc/freetype")

# Link Vulkan from SDK
# Link Libraries
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	# Using VVL
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
		target_compile_definitions(${PROJECT_NAME} PRIVATE PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
	endif()

	# build Vulkan
	target_link_libraries(${PROJECT_NAME} PRIVATE vulkan Vulkan::Headers)
else()
	find_package(Vulkan REQUIRED)

	set (VULKANSDK_LIBS_PATH "${Vulkan_LIBRARIES}")
	string(REPLACE "vulkan-1.lib" "" VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})
	string(STRIP VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})

    target_link_libraries(${PROJECT_NAME} PRIVATE
		-lvulkan-1
		-L${VULKANSDK_LIBS_PATH})

    target_include_directories(${PROJECT_NAME} PRIVATE "${Vulkan_INCLUDE_DIRS}")
endif()

if (TARGET asm_offset)
	set_target_properties(asm_offset
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if (LIBS_SHARED_BUILD)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE freetype)
target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	VERSION "${Imgui_VERSION}")

target_sources(${PROJECT_NAME} PRIVATE
	"imgui.cpp"
	"imgui_demo.cpp"
	"imgui_draw.cpp"
	"imgui_tables.cpp"
	"imgui_widgets.cpp"
	"backends/imgui_impl_sdl2.cpp"
	"backends/imgui_impl_vulkan.cpp"
	"misc/freetype/imgui_freetype.cpp")
