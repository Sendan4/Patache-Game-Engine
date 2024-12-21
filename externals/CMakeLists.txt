# Externals / Thrid Party Libraries

message("")
message("  - Fast_io -")

include(CheckIncludeFileCXX)
string(APPEND CMAKE_REQUIRED_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/fast_io/include")
CHECK_INCLUDE_FILE_CXX("fast_io.h" HAVE_FAST_IO)
if(NOT HAVE_FAST_IO)
    message(FATAL_ERROR "fast_io.h not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::print(\"Hello, World!\"); return 0; }"
    HAVE_FAST_IO_PRINT)
if(NOT HAVE_FAST_IO_PRINT)
    message(FATAL_ERROR "fast_io::io::print not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::println(\"Hello, World!\"); return 0; }"
    HAVE_FAST_IO_PRINTLN)
if(NOT HAVE_FAST_IO_PRINTLN)
    message(FATAL_ERROR "fast_io::io::println not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::perr(\"Hello, World!\"); return 0; }"
    HAVE_FAST_IO_PERR)
if(NOT HAVE_FAST_IO_PERR)
    message(FATAL_ERROR "fast_io::io::perr not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::perrln(\"Hello, World!\"); return 0; }"
    HAVE_FAST_IO_PERRLN)
if(NOT HAVE_FAST_IO_PERRLN)
    message(FATAL_ERROR "fast_io::io::perrln not usable")
endif()

CHECK_INCLUDE_FILE_CXX("fast_io_device.h" HAVE_FAST_IO_DEVICE)
if(NOT HAVE_FAST_IO_DEVICE)
    message(FATAL_ERROR "fast_io_device.h not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io_device.h>\n\
    int main() { fast_io::dir_file Directory(\"Test/\"); return 0; }"
    HAVE_FAST_IO_DIR_FILE)
if(NOT HAVE_FAST_IO_DIR_FILE)
    message(FATAL_ERROR "fast_io::dir_file not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io_device.h>\n\
    int main(void) { fast_io::native_file_loader loader(\"test.yaml\"); return 0; }"
    HAVE_FAST_IO_NATIVE_FILE_LOADER)
if(NOT HAVE_FAST_IO_NATIVE_FILE_LOADER)
    message(FATAL_ERROR "fast_io::native_file_loader not usable")
endif()

add_subdirectory(fast_io)

# RapidYaml
message("")
message("  - RapidYaml -")

set (RYML_DEV OFF CACHE INTERNAL "..." FORCE)
set (RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
set (RYML_INSTALL OFF CACHE INTERNAL "..." FORCE)

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
    set (VULKAN_HEADERS_ENABLE_MODULE OFF CACHE INTERNAL "..." FORCE)

	add_subdirectory(Vulkan-Headers)
	message(STATUS "Header-Only")

	# Vulkan
	message("")
	message("  - Vulkan Loader -")
	add_subdirectory(Vulkan-Loader)

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

# FreeType
message("")
message("  - FreeType -")
add_subdirectory(freetype)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	add_subdirectory(imgui)
endif()
