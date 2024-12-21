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
else ()
	message(STATUS "Library Type : Static")
	add_library(${PROJECT_NAME} STATIC)

    if (SHARED_BUILD AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE "-fPIC")
		message(STATUS "Build With Position Independent Code")
	endif()
endif()

if (NOT MSVC OR MSYS AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
	enable_language(RC)
    target_sources(${PROJECT_NAME} PRIVATE "imgui.rc")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ".")
target_include_directories(${PROJECT_NAME} PUBLIC "backends/")
target_include_directories(${PROJECT_NAME} PUBLIC "misc/freetype")

if(USE_EXTERNAL_LIBS AND HAVE_VULKAN_H)
	target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Vulkan-Headers/include/")
endif()

target_sources(${PROJECT_NAME} PRIVATE
	"imgui.cpp"
	"imgui_demo.cpp"
	"imgui_draw.cpp"
	"imgui_tables.cpp"
	"imgui_widgets.cpp"
	"backends/imgui_impl_sdl2.cpp"
	"backends/imgui_impl_vulkan.cpp"
	"misc/freetype/imgui_freetype.cpp")

if (USE_EXTERNAL_LIBS)
	# Linux or MSVC
	if (MSVC OR LINUX OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
		target_link_libraries(${PROJECT_NAME} PUBLIC vulkan Vulkan::Headers)

		set_target_properties(asm_offset
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	else()
		# mingw
		message("")
		message("  - Vulkan SDK -")
		find_package(Vulkan REQUIRED)

		set (VULKANSDK_LIBS_PATH "${Vulkan_LIBRARIES}")
		string(REPLACE "vulkan-1.lib" "" VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})
		string(STRIP VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})
		message(STATUS "Vulkan Libs Path : ${VULKANSDK_LIBS_PATH}")

		target_link_libraries(${PROJECT_NAME} PUBLIC
			-lvulkan-1
			-L${VULKANSDK_LIBS_PATH})

		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
			target_compile_definitions(${PROJECT_NAME} PUBLIC VVL_FROM_SDK=1)
		endif()
		target_include_directories(${PROJECT_NAME} PUBLIC "${Vulkan_INCLUDE_DIRS}")
	endif()

	if (LIBS_SHARED_BUILD)
		target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
	else()
		target_link_libraries(${PROJECT_NAME} PUBLIC SDL2-static)
	endif()

	target_link_libraries(${PROJECT_NAME} PUBLIC freetype)
else()
	# Package Mananger
	target_link_libraries(${PROJECT_NAME} PRIVATE
		${Vulkan_LIBRARIES}
		Vulkan::Headers
		${SDL2_LIBRARIES}
		freetype)
endif()

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	VERSION "${Imgui_VERSION}")
