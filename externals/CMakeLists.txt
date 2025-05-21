# Externals / Thrid Party Libraries
set (FETCHCONTENT_QUIET OFF)
include(FetchContent)

message("")
message("  - Fast_io -")

include(CheckIncludeFileCXX)
string(APPEND CMAKE_REQUIRED_FLAGS "")
string(APPEND CMAKE_REQUIRED_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/fast_io/include")
CHECK_INCLUDE_FILE_CXX("fast_io.h" HAVE_FAST_IO)
if(NOT HAVE_FAST_IO)
    message(FATAL_ERROR "fast_io.h not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::print(\"Hello, World!\"); return 0; }"
	fast_io_io_print)
if(NOT fast_io_io_print)
    message(FATAL_ERROR "fast_io::io::print not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::println(\"Hello, World!\"); return 0; }"
    fast_io_io_println)
if(NOT fast_io_io_println)
    message(FATAL_ERROR "fast_io::io::println not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::perr(\"Hello, World!\"); return 0; }"
    fast_io_io_perr)
if(NOT fast_io_io_perr)
    message(FATAL_ERROR "fast_io::io::perr not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io.h>\n\
    int main() { fast_io::io::perrln(\"Hello, World!\"); return 0; }"
    fast_io_io_perrln)
if(NOT fast_io_io_perrln)
    message(FATAL_ERROR "fast_io::io::perrln not usable")
endif()

CHECK_INCLUDE_FILE_CXX("fast_io_device.h" HAVE_FAST_IO_DEVICE)
if(NOT HAVE_FAST_IO_DEVICE)
    message(FATAL_ERROR "fast_io_device.h not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io_device.h>\n\
    int main() { fast_io::dir_file Directory(\"Test/\"); return 0; }"
    fast_io_dir_file)
if(NOT fast_io_dir_file)
    message(FATAL_ERROR "fast_io::dir_file not usable")
endif()

check_cxx_source_compiles(
    "#include <fast_io_device.h>\n\
    int main(void) { fast_io::native_file_loader loader(\"test.yaml\"); return 0; }"
    fast_io_native_file_loader)
if(NOT fast_io_native_file_loader)
    message(FATAL_ERROR "fast_io::native_file_loader not usable")
endif()

add_subdirectory(fast_io)

# RapidYaml
message("")
message("  - RapidYaml -")

set (RYML_DEV OFF CACHE INTERNAL "..." FORCE)
set (RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
set (RYML_INSTALL OFF CACHE INTERNAL "..." FORCE)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/rapidyaml/)
  add_subdirectory(rapidyaml)
else()
  FetchContent_Declare(
    RapidYaml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG v0.9.0
  )
  FetchContent_MakeAvailable(RapidYaml)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	# Linux Wayland / Xorg
	# Wayland Enable by default
	set (BUILD_WSI_WAYLAND_SUPPORT ON CACHE INTERNAL "..." FORCE)
	set (BUILD_WSI_XCB_SUPPORT OFF CACHE BOOL "..." FORCE)
	set (BUILD_WSI_XLIB_SUPPORT OFF CACHE BOOL "..." FORCE)
endif()

if (MSVC OR LINUX)
	set (BUILD_TESTS OFF CACHE INTERNAL "..." FORCE)
  set (UPDATE_DEPS ON CACHE INTERNAL "..." FORCE)
  set (VULKAN_HEADERS_ENABLE_MODULE OFF CACHE INTERNAL "..." FORCE)

	message("")
	message("  - Vulkan Headers -")
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers/)
    add_subdirectory(Vulkan-Headers)
  else()
    FetchContent_Declare(
      Vulkan-Headers
      GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
      GIT_TAG v1.4.313
    )
    FetchContent_MakeAvailable(Vulkan-Headers)
  endif()

	message("")
	message("  - Vulkan Loader -")
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Loader/)
    add_subdirectory(Vulkan-Loader)
  else()
    FetchContent_Declare(
      Vulkan-Loader
      GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
      GIT_TAG v1.4.313
    )
    FetchContent_MakeAvailable(Vulkan-Loader)
  endif()

  # Vulkan Validation Layer
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
    message("")
	  message("  - Vulkan Validation Layers -")

    FetchContent_Declare(
      Vulkan-ValidationLayers
      GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers.git
      GIT_TAG v1.4.313
    )

	  FetchContent_MakeAvailable(Vulkan-ValidationLayers)
	endif()
endif()

message("")
message("  - Glslang -")
set	(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set	(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (ENABLE_RTTI OFF CACHE INTERNAL "..." FORCE)
set (ENABLE_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
set (GLSLANG_TESTS OFF CACHE INTERNAL "..." FORCE)
set (ENABLE_HLSL OFF CACHE INTERNAL "..." FORCE)
set (ENABLE_PCH OFF CACHE INTERNAL "..." FORCE)
set (GLSLANG_ENABLE_INSTALL OFF CACHE INTERNAL "..." FORCE)
set (ENABLE_SPVREMAPPER OFF CACHE INTERNAL "..." FORCE)
set (ALLOW_EXTERNAL_SPIRV_TOOLS OFF CACHE INTERNAL "..." FORCE)
set (BUILD_EXTERNAL ON CACHE INTERNAL "..." FORCE)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glslang)
  execute_process(
	  COMMAND python3 update_glslang_sources.py
	  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glslang)

  add_subdirectory(glslang)
else() 
  set (BUILD_EXTERNAL OFF CACHE INTERNAL "..." FORCE)
  set (ENABLE_OPT OFF CACHE INTERNAL "..." FORCE)

  FetchContent_Declare(
    Glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG vulkan-sdk-1.4.309.0
  )

  FetchContent_MakeAvailable(Glslang)
endif()
set	(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set	(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#cglm
message("")
message("  - cglm -")
set (CGLM_USE_C99 OFF CACHE INTERNAL "..." FORCE)
set (CGLM_USE_TEST OFF CACHE INTERNAL "..." FORCE)
if (BUILD_SHARED_LIBS)
  set (CGLM_SHARED ON CACHE INTERNAL "..." FORCE)
  set (CGLM_STATIC OFF CACHE INTERNAL "..." FORCE)
else()
  set (CGLM_SHARED OFF CACHE INTERNAL "..." FORCE)
  set (CGLM_STATIC ON CACHE INTERNAL "..." FORCE)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cglm/)
  add_subdirectory(cglm)
else() 
  FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG v0.9.6
  )
  FetchContent_MakeAvailable(cglm)
endif()

# Wayland Client
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message("")
  message("  - Wayland -")

	find_package(PkgConfig REQUIRED)
	pkg_check_modules(WAYLAND wayland-client)
	find_program(WaylandScanner
		wayland-scanner
		REQUIRED)

	message(STATUS "Wayland Scanner : ${WaylandScanner}")

	execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory WaylandProtocols
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	if (EXISTS ${CMAKE_BINARY_DIR}/WaylandProtocols)
		# xdg-shell protocol
		message(STATUS "Protocol : xdg_shell")

		file(DOWNLOAD
			https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/810f1adaf33521cc55fc510566efba2a1418174f/stable/xdg-shell/xdg-shell.xml?inline=false
			${CMAKE_BINARY_DIR}/WaylandProtocols/xdg-shell.xml
			SHOW_PROGRESS
		)

		execute_process(
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/WaylandProtocols/
			COMMAND wayland-scanner client-header xdg-shell.xml xdg-shell.h
			COMMAND wayland-scanner private-code xdg-shell.xml xdg-shell.c)

		# xdg-decoration-unstable-v1.xml protocol
		message(STATUS "Protocol : xdg-decoration-unstable-v1.xml")

		file(DOWNLOAD
			https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml?ref_type=heads&inline=false
			${CMAKE_BINARY_DIR}/WaylandProtocols/xdg-decoration-unstable-v1.xml
			SHOW_PROGRESS
		)

		execute_process(
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/WaylandProtocols/
			COMMAND wayland-scanner client-header xdg-decoration-unstable-v1.xml xdg-decoration-unstable-v1.h
			COMMAND wayland-scanner private-code xdg-decoration-unstable-v1.xml xdg-decoration-unstable-v1.c)
	endif()
endif()

# SDL2
message("")
message("  - SDL3 -")

if (BUILD_SHARED_LIBS)
  set(SDL_SHARED ON CACHE INTERNAL "..." FORCE)
  set(SDL_STATIC OFF CACHE INTERNAL "..." FORCE)
else ()
  set(SDL_SHARED OFF CACHE INTERNAL "..." FORCE)
  set(SDL_STATIC ON CACHE INTERNAL "..." FORCE)
endif ()

set(SDL_STATIC_PIC ON CACHE INTERNAL "..." FORCE)

set(SDL_UNUSED_SUBSYSTEMS
  File
  Locale
	Power
	Render
  Gpu
	Sensor
	Timers
	Atomic
	Audio
  Camera)
foreach(_SUB ${SDL_UNUSED_SUBSYSTEMS})
	string(TOUPPER ${_SUB} _OPT)
	set(SDL_${_OPT} OFF CACHE INTERNAL "..." FORCE)
endforeach()

set (SDL_INSTALL OFF CACHE INTERNAL "..." FORCE)
set (SDL_INSTALL_CPACK OFF CACHE INTERNAL "..." FORCE)
set (SDL_ASSERTIONS ON CACHE INTERNAL "..." FORCE)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
set (SDL_ASSERTIONS "paranoid" CACHE INTERNAL "..." FORCE)
else()
set (SDL_ASSERTIONS "release" CACHE INTERNAL "..." FORCE)
endif()
set (SDL_TEST OFF CACHE INTERNAL "..." FORCE)
set (SDL_DIRECTX OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER_D3D OFF CACHE INTERNAL "..." FORCE)
set (SDL_OPENGL OFF CACHE INTERNAL "..." FORCE)
set (SDL_OPENGLES OFF CACHE INTERNAL "..." FORCE)
set (SDL_OSS OFF CACHE INTERNAL "..." FORCE)
set (SDL_ALSA OFF CACHE INTERNAL "..." FORCE)
set (SDL_ALSA_SHARED OFF CACHE INTERNAL "..." FORCE)
set (SDL_JACK OFF CACHE INTERNAL "..." FORCE)
set (SDL_JACK_SHARED OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER_VULKAN OFF CACHE INTERNAL "..." FORCE)
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
set (SDL_RENDER_GPU OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER_D3D OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER_D3D11 OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER_D3D12 OFF CACHE INTERNAL "..." FORCE)
set (SDL_RENDER_METAL OFF CACHE INTERNAL "..." FORCE)
set (SDL_GPU_DXVK OFF CACHE INTERNAL "..." FORCE)
set (SDL_TEST_LIBRARY OFF CACHE INTERNAL "..." FORCE)
set (SDL_WAYLAND_LIBDECOR OFF CACHE INTERNAL "..." FORCE)
set (SDL_WAYLAND_LIBDECOR_SHARED OFF CACHE INTERNAL "..." FORCE)
set (SDL_DBUS OFF CACHE INTERNAL "..." FORCE)
set (SDL_IBUS OFF CACHE INTERNAL "..." FORCE)
set (SDL_VENDOR_INFO ${PROJECT_NAME} CACHE STRING "Vendor name and/or version to add to SDL_REVISION")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set (SDL_X11 OFF CACHE INTERNAL "..." FORCE)
	set (SDL_X11_SHARED OFF CACHE INTERNAL "..." FORCE)
endif ()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SDL/)
  add_subdirectory(SDL)
else() 
  FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.10
  )
  FetchContent_MakeAvailable(SDL)
endif()

# FreeType
message("")
message("  - FreeType -")
set (FT_DISABLE_ZLIB ON CACHE INTERNAL "..." FORCE)
set (FT_DISABLE_BZIP2 ON CACHE INTERNAL "..." FORCE)
set (FT_DISABLE_PNG ON CACHE INTERNAL "..." FORCE)
set (FT_DISABLE_HARFBUZZ ON CACHE INTERNAL "..." FORCE)
set (FT_DISABLE_BROTLI ON CACHE INTERNAL "..." FORCE)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/freetype/)
  add_subdirectory(freetype)
else() 
  FetchContent_Declare(
    FreeType
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG VER-2-13-3
  )
  FetchContent_MakeAvailable(FreeType)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	add_subdirectory(imgui)
endif()
