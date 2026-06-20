# Externals / Thrid Party Libraries
set (FETCHCONTENT_QUIET OFF)
include(FetchContent)

add_subdirectory(fast_io)

# RapidYaml
message("")
message("  - RapidYaml -")

set (RYML_DEV OFF CACHE INTERNAL "..." FORCE)
set (RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
set (RYML_INSTALL OFF CACHE INTERNAL "..." FORCE)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/rapidyaml/CMakeLists.txt)
    add_subdirectory(rapidyaml)
else()
    FetchContent_Declare(
        RapidYaml
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
        GIT_TAG v0.15.2
    )
    FetchContent_MakeAvailable(RapidYaml)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "DragonFly" OR
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    # Linux and Unix Wayland only
    set (BUILD_WSI_WAYLAND_SUPPORT ON CACHE INTERNAL "..." FORCE)
    set (BUILD_WSI_XCB_SUPPORT OFF CACHE BOOL "..." FORCE)
    set (BUILD_WSI_XLIB_SUPPORT OFF CACHE BOOL "..." FORCE)
endif()

message("")
message("  - Vulkan Headers -")
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers/CMakeLists.txt)
    add_subdirectory(Vulkan-Headers)
else()
    FetchContent_Declare(
        Vulkan-Headers
      	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
      	GIT_TAG vulkan-sdk-1.4.350.0)

    FetchContent_MakeAvailable(Vulkan-Headers)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR
    CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "DragonFly" OR
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    set (BUILD_TESTS OFF CACHE INTERNAL "..." FORCE)
    set (UPDATE_DEPS ON CACHE INTERNAL "..." FORCE)
    set (VULKAN_HEADERS_ENABLE_MODULE OFF CACHE INTERNAL "..." FORCE)
    
    # Vulkan Validation Layer
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
        message("")
        message("  - Vulkan Validation Layers -")

        FetchContent_Declare(
            Vulkan-ValidationLayers
            GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers.git
            GIT_TAG vulkan-sdk-1.4.350.0
        )
    
        FetchContent_MakeAvailable(Vulkan-ValidationLayers)
    endif()

    message("")
    message("  - Vulkan Loader -")
   
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Loader/CMakeLists.txt)
        add_subdirectory(Vulkan-Loader)
    else()
    	  FetchContent_Declare(
            Vulkan-Loader
            GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
            GIT_TAG vulkan-sdk-1.4.350.0
    	  )
    	  FetchContent_MakeAvailable(Vulkan-Loader)
    endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"
    AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    include(../cmake/WindowsVulkanSDK.cmake)
endif()

message("")
message("  - Vulkan Memory Allocator -")
set (VMA_BUILD_SAMPLES OFF CACHE BOOL "..." FORCE)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/CMakeLists.txt)
    add_subdirectory(VulkanMemoryAllocator)
else()
    FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.3.0
    )
    FetchContent_MakeAvailable(VulkanMemoryAllocator)
endif()

message("")
message("  - Vulkan Utility Libraries -")

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Utility-Libraries/CMakeLists.txt)
    set (UPDATE_DEPS ON CACHE INTERNAL "..." FORCE)
    add_subdirectory(Vulkan-Utility-Libraries)
else()
    set (UPDATE_DEPS ON CACHE INTERNAL "..." FORCE)

    FetchContent_Declare(
        Vulkan-Utility-Libraries
        GIT_REPOSITORY https://codeberg.org/PatacheEngine/Vulkan-Utility-Libraries.git
        GIT_TAG v1.4.346-Patache
    )

    FetchContent_MakeAvailable(Vulkan-Utility-Libraries)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR
    CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "DragonFly" OR
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    message("")
    message("  - Glslang -")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set (ENABLE_RTTI OFF CACHE INTERNAL "..." FORCE)
    set (ENABLE_EXCEPTIONS OFF CACHE INTERNAL "..." FORCE)
    set (GLSLANG_TESTS OFF CACHE INTERNAL "..." FORCE)
    set (ENABLE_HLSL OFF CACHE INTERNAL "..." FORCE)
    set (ENABLE_PCH OFF CACHE INTERNAL "..." FORCE)
    set (GLSLANG_ENABLE_INSTALL OFF CACHE INTERNAL "..." FORCE)
    set (ENABLE_SPVREMAPPER OFF CACHE INTERNAL "..." FORCE)
    set (ALLOW_EXTERNAL_SPIRV_TOOLS OFF CACHE INTERNAL "..." FORCE)
    set (BUILD_EXTERNAL ON CACHE INTERNAL "..." FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glslang/CMakeLists.txt)
        find_package(Python 3 REQUIRED COMPONENTS Interpreter)

        execute_process(
            COMMAND ${Python_EXECUTABLE} update_glslang_sources.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glslang)

        add_subdirectory(glslang)
    else()
        set (BUILD_EXTERNAL OFF CACHE INTERNAL "..." FORCE)
        set (ENABLE_OPT OFF CACHE INTERNAL "..." FORCE)

        FetchContent_Declare(
            Glslang
            GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
            GIT_TAG vulkan-sdk-1.4.350.0
        )

        FetchContent_MakeAvailable(Glslang)
    endif()

    get_filename_component(BUILDDIR_NAME ${CMAKE_BINARY_DIR} NAME)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILDDIR_NAME})
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILDDIR_NAME})
endif()

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

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cglm/CMakeLists.txt)
    add_subdirectory(cglm)
else() 
    FetchContent_Declare(
        cglm
        GIT_REPOSITORY https://github.com/recp/cglm.git
        GIT_TAG v0.9.6
    )
    FetchContent_MakeAvailable(cglm)
endif()

# Wayland Client and Cursor
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "DragonFly" OR
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    message("")
    message("  - Wayland -")
  
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND wayland-client REQUIRED)
    pkg_check_modules(WAYLAND_CURSOR wayland-cursor REQUIRED)

	if (${WAYLAND_FOUND})
        message(STATUS "wayland-client has been found")
    endif()

	if (${WAYLAND_CURSOR_FOUND})
        message(STATUS "wayland-cursor has been found")
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

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "DragonFly" OR
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    set (SDL_X11 OFF CACHE INTERNAL "..." FORCE)
    set (SDL_X11_SHARED OFF CACHE INTERNAL "..." FORCE)
endif ()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SDL/CMakeLists.txt)
    add_subdirectory(SDL)
else() 
    FetchContent_Declare(
        SDL
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.2
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

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/freetype/CMakeLists.txt)
    add_subdirectory(freetype)
else() 
    FetchContent_Declare(
        FreeType
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-14-1
    )
    FetchContent_MakeAvailable(FreeType)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_subdirectory(imgui)
endif()
