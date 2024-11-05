# Externals / Thrid Party Libraries

add_subdirectory(fast_io)

# Yaml-cpp
message("")
message("  - RapidYaml -")

set (RYML_DEV OFF CACHE INTERNAL "..." FORCE)
set (RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
set (RYML_INSTALL OFF CACHE INTERNAL "..." FORCE)

if (LIBS_SHARED_BUILD)
	set (YAML_BUILD_SHARED_LIBS ON CACHE INTERNAL "..." FORCE)
	message(STATUS "Library Type : Shared")
else ()
	message(STATUS "Library Type : Static")
endif()

add_subdirectory(rapidyaml)

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

if (MSVC OR LINUX)
	# Vulkan Headers
	message("")
	message("  - Vulkan Headers -")
	set (BUILD_TESTS OFF CACHE INTERNAL "..." FORCE)
    set (UPDATE_DEPS ON CACHE INTERNAL "..." FORCE)

	add_subdirectory(Vulkan-Headers)
	message(STATUS "Header-Only")

	# Vulkan Validation Layer
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
        message("")
	    message("  - Vulkan ValidationLayers -")

	    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR UNIX)
	       message(STATUS "BUILD_WSI_XLIB_SUPPORT : ${BUILD_WSI_XLIB_SUPPORT}")
	       message(STATUS "BUILD_WSI_XCB_SUPPORT : ${BUILD_WSI_XCB_SUPPORT}")
	       message(STATUS "BUILD_WSI_WAYLAND_SUPPORT : ${BUILD_WSI_WAYLAND_SUPPORT}")
		endif()

		set(PATATA_VULKAN_VALIDATION_LAYERS_VERSION "vulkan-sdk-1.3.296")

        include(FetchContent)
        FetchContent_Populate(
            Vulkan-ValidationLayers
            GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers.git
            GIT_TAG ${PATATA_VULKAN_VALIDATION_LAYERS_VERSION}
            BUILD_IN_SOURCE FALSE
            GIT_PROGRESS TRUE
            LOG_DOWNLOAD TRUE
            LOG_UPDATE TRUE
            LOG_CONFIGURE TRUE
            LOG_BUILD TRUE
            LOG_INSTALL TRUE
            GIT_SUBMODULES_RECURSE TRUE
            CMAKE_ARGS
                "-DUPDATE_DEPS=ON
                -DBUILD_WSI_XLIB_SUPPORT=${BUILD_WSI_XLIB_SUPPORT}
                -DBUILD_WSI_XCB_SUPPORT=${BUILD_WSI_XCB_SUPPORT}
                -DBUILD_WSI_WAYLAND_SUPPORT=${BUILD_WSI_WAYLAND_SUPPORT}
                -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                -DRUNTIME_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
            INSTALL_COMMAND ""
        )

        add_subdirectory(${vulkan-validationlayers_SOURCE_DIR} ${vulkan-validationlayers_BINARY_DIR})
	endif()

	# Vulkan
	message("")
	message("  - Vulkan Loader -")
	add_subdirectory(Vulkan-Loader)
endif()

# SDL2
message("")
message("  - SDL2 -")

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
set (SDL_OPENGLES OFF CACHE INTERNAL "..." FORCE)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if (NOT XORG_SUPPORT)
		set (SDL_X11 OFF CACHE INTERNAL "..." FORCE)
		set (SDL_X11_SHARED OFF CACHE INTERNAL "..." FORCE)
	endif ()
endif ()

add_subdirectory(SDL)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	add_subdirectory(imgui)
endif()

# checks
message("")
message("  - Patata Engine -")

include(CheckCXXSymbolExists)

find_file(HAVE_VULKAN_HPP "vulkan/vulkan.hpp" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers/include/")
find_file(HAVE_SDL_H "SDL.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include/")
find_file(HAVE_SDL_VULKAN_H "SDL_vulkan.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include/")
find_file(HAVE_YAML_CPP_H "yaml-cpp/yaml.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include/")

if (NOT HAVE_SDL_H OR NOT HAVE_SDL_VULKAN_H OR NOT HAVE_VULKAN_HPP OR NOT HAVE_YAML_CPP_H)
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

	if (HAVE_YAML_CPP_H)
		message(STATUS "Looking for yaml-cpp/yaml.h - found")
	endif()
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	find_file(HAVE_IMGUI_VULKAN_H "imgui_impl_vulkan.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends")
	find_file(HAVE_IMGUI_SDL2_H "imgui_impl_sdl2.h" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends")

	if (NOT HAVE_IMGUI_VULKAN_H OR NOT HAVE_IMGUI_SDL2_H)
		message(FATAL_ERROR "Missing Headers")
	else()
		if (HAVE_IMGUI_VULKAN_H)
			message(STATUS "Looking for imgui_impl_vulkan.h - found")
		endif()

		if (HAVE_IMGUI_SDL2_H)
			message(STATUS "Looking for imgui_impl_sdl2.h - found")
		endif()
	endif()
endif()
