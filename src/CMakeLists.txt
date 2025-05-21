if (BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

add_subdirectory(RaccoonRenderer/Shaders)

add_dependencies(${PROJECT_NAME} PatacheFiles PatacheCompileShaders)

target_sources (${PROJECT_NAME} PRIVATE
 "Core/PatacheEngine.cpp"
 "Core/Configuration.cpp"
 "Core/Input.cpp"
 "Core/ClearColor.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_SetupRenderer.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_SwapChain.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_ImageView.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_CommandBuffer.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_FrameBuffer.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_DepthBuffer.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_SincronizationPrimitives.cpp"
 "RaccoonRenderer/Vulkan/Vulkan_Render.cpp"
 "Log/Messages.cpp"
 "Log/StartLogInfo.cpp"
 "Log/Vulkan_SetupLog.cpp"
 "Log/Vulkan_Info.cpp")

if (MSVC)
    # List Headers Files in Visual Studio
    file(GLOB_RECURSE Include "${CMAKE_CURRENT_SOURCE_DIR}/../include/PatacheEngine/*.hpp")
    file(GLOB_RECURSE IncludeInternal "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

    target_sources(${PROJECT_NAME} PRIVATE ${Include} ${IncludeInternal})
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" OR WIN32)
    enable_language(RC)

    target_sources (${PROJECT_NAME} PRIVATE "PatacheEngine.rc")
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	enable_language(C)

	target_sources (${PROJECT_NAME} PRIVATE
		"Core/WaylandWindow.cpp"
		${CMAKE_BINARY_DIR}/WaylandProtocols/xdg-shell.h
		${CMAKE_BINARY_DIR}/WaylandProtocols/xdg-shell.c
		${CMAKE_BINARY_DIR}/WaylandProtocols/xdg-decoration-unstable-v1.h
		${CMAKE_BINARY_DIR}/WaylandProtocols/xdg-decoration-unstable-v1.c)

	target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/WaylandProtocols/)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_sources(${PROJECT_NAME} PRIVATE "ImGui/SetupImgui.cpp")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Core/"
  "${CMAKE_CURRENT_SOURCE_DIR}/ImGui/"
  "${CMAKE_CURRENT_SOURCE_DIR}/Log/"
  "${CMAKE_CURRENT_SOURCE_DIR}/RaccoonRenderer/")

# Compiler Flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GNU & Clang
    if (MINGW AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo") AND COMPILER_SUPPORTS_FPERMISSIVE)
        # Mingw specific function conversion in vulkan
        target_compile_options(${PROJECT_NAME} PRIVATE "-fpermissive")
    endif()
    if (COMPILER_SUPPORTS_WALL)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    endif ()
    if (COMPILER_SUPPORTS_WEXTRA)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
    endif ()
    if (COMPILER_SUPPORTS_WPEDANTIC)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wpedantic")
    endif ()
    if (COMPILER_SUPPORTS_NO_EXCEPTIONS)
        target_compile_options(${PROJECT_NAME} PRIVATE "-fno-exceptions")
    endif ()
    if (COMPILER_SUPPORTS_NO_STRICT_ALIASING)
        target_compile_options(${PROJECT_NAME} PRIVATE "-fno-strict-aliasing")
    endif ()
    if (COMPILER_SUPPORTS_NO_RTTI)
        target_compile_options(${PROJECT_NAME} PRIVATE "-fno-rtti")
    endif()
    if (COMPILER_SUPPORTS_FAST_MATH)
        target_compile_options(${PROJECT_NAME} PRIVATE "-ffast-math")
    endif()
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
      if (COMPILER_SUPPORTS_SSE4.2)
        target_compile_options(${PROJECT_NAME} PRIVATE "-msse4.2")
      endif()
      if (COMPILER_SUPPORTS_MMX)
        target_compile_options(${PROJECT_NAME} PRIVATE "-mmmx")
      endif()
      if (COMPILER_SUPPORTS_AVX)
        target_compile_options(${PROJECT_NAME} PRIVATE "-mavx")
      endif()
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC
    if (COMPILER_SUPPORTS_W4)
        target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
    endif ()
    if (COMPILER_SUPPORTS_NO_RTTI)
        target_compile_options(${PROJECT_NAME} PRIVATE "/GR-")
    endif()
    if (COMPILER_SUPPORTS_FLOATING_POINT_FAST)
        target_compile_options(${PROJECT_NAME} PRIVATE "/fp:fast")
    endif()
    if (COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT)
        target_compile_options(${PROJECT_NAME} PRIVATE "/fp:except-")
    endif()
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
      if (COMPILER_SUPPORTS_SSE4.2)
        target_compile_options(${PROJECT_NAME} PRIVATE "/arch:SSE4.2")
      endif()
      if (COMPILER_SUPPORTS_AVX)
        target_compile_options(${PROJECT_NAME} PRIVATE "/arch:AVX")
      endif()
    endif()

    target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=0)
endif()

# Macros
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_BUILD_TYPE="Release")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATACHE_DEBUG=0)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_BUILD_TYPE="Debug")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATACHE_DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_BUILD_TYPE="Debug Optimized")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATACHE_DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_BUILD_TYPE="Minimal Size Release")
  target_compile_definitions(${PROJECT_NAME} PUBLIC PATACHE_DEBUG=0)
endif()

if (NOT "${CMAKE_GENERATOR_VERSION}" STREQUAL "")
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    PATACHE_BUILD_SYSTEM_GENERATOR_VERSION="${CMAKE_GENERATOR_VERSION}")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
 PATACHE_BUILD_SYSTEM="CMake"
 PATACHE_BUILD_SYSTEM_VERSION="${CMAKE_VERSION}"
 PATACHE_BUILD_SYSTEM_GENERATOR="${CMAKE_GENERATOR}"
 PATACHE_ENGINE_VERSION="${PATACHE_ENGINE_VERSION}"
 PATACHE_ENGINE_VERSION_MAYOR=${PATACHE_ENGINE_VERSION_MAYOR}
 PATACHE_ENGINE_VERSION_MINOR=${PATACHE_ENGINE_VERSION_MINOR}
 PATACHE_ENGINE_VERSION_PATCH=${PATACHE_ENGINE_VERSION_PATCH}
 PATACHE_ENGINE_NAME="Patache Engine"
 PATACHE_COMPILER_PROGRAM="${compiler_program}"
 PATACHE_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
 PATACHE_ARCH="${CMAKE_SYSTEM_PROCESSOR}"
 PATACHE_FAST_IO_GIT_COMMIT_HASH_SHORT="${Fast_io_COMMIT_SHORT_HASH}"
 VULKAN_HPP_NO_EXCEPTIONS
 VULKAN_HPP_NO_SMART_HANDLE
 VULKAN_HPP_NO_CONSTRUCTORS)

if (MSVC OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=0)
endif()

# Git
if (USE_GIT AND GIT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PATACHE_GIT_BRANCH="${PATACHE_GIT_BRANCH}"
        PATACHE_GIT_HASH_LONG="${PATACHE_GIT_HASH_LONG}"
        PATACHE_GIT_HASH_SHORT="${PATACHE_GIT_HASH_SHORT}"
        PATACHE_GIT_WORK_DIR_IS_CLEAN="${PATACHE_GIT_WORK_DIR_IS_CLEAN}"
        PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL=${PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL}
        PATACHE_GIT_WORK_DIR_IS_STAGED="${PATACHE_GIT_WORK_DIR_IS_STAGED}"
        PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL=${PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL})
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_IMGUI_VERSION="${Imgui_VERSION}")
endif()

if (USE_VVL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_USE_VVL)
endif()

# Link Libraries
# Wayland
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PRIVATE ${WAYLAND_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PRIVATE ${WAYLAND_INCLUDE_DIRS})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    # Using VVL
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_FIND_VVL_IN_THE_CURRENT_PATH)
    endif()

    target_link_libraries(${PROJECT_NAME} PUBLIC
			vulkan
			Vulkan::Headers)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Link Vulkan from SDK
    message("")
    message("  - Vulkan SDK -")
    find_package(Vulkan REQUIRED)

    set (VULKANSDK_LIBS_PATH "${Vulkan_LIBRARIES}")
    string(REPLACE "/vulkan-1.lib" "" VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})
    string(STRIP VULKANSDK_LIBS_PATH ${VULKANSDK_LIBS_PATH})

    message(STATUS "Vulkan Libs Path : ${VULKANSDK_LIBS_PATH}")

    target_link_libraries(${PROJECT_NAME} PUBLIC
        -lvulkan-1
        -L${VULKANSDK_LIBS_PATH})

    target_include_directories(${PROJECT_NAME} PUBLIC "${Vulkan_INCLUDE_DIRS}")

    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
        set (VULKANSDK_VVL_PATH "${Vulkan_LIBRARIES}")
        string(REPLACE "/Lib/vulkan-1.lib" "" VULKANSDK_VVL_PATH ${VULKANSDK_VVL_PATH})
        string(STRIP VULKANSDK_VVL_PATH ${VULKANSDK_VVL_PATH})
        set (VULKANSDK_VVL_PATH "${VULKANSDK_VVL_PATH}/Bin")
        message(STATUS "Validation Layer Path : ${VULKANSDK_VVL_PATH}")

        target_compile_definitions(${PROJECT_NAME} PRIVATE
          PATACHE_FIND_VVL_FROM_SDK PATACHE_VVL_SDK_PATH="${VULKANSDK_VVL_PATH}")
    endif()
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_WAYLAND)
    if (XORG_SUPPORT)
        target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_X11)
    endif()
endif()

if (TARGET asm_offset)
    set_target_properties(asm_offset
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC Fast_io SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE ryml::ryml freetype cglm)
target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_link_libraries(${PROJECT_NAME} PUBLIC Imgui)
endif()

# MSVC Folders
if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")
    set_target_properties(PatacheCompileShaders PROPERTIES FOLDER "Engine")

    if (TARGET cglm)
      set_target_properties(cglm PROPERTIES FOLDER "Engine/Dependencies/")
    endif()

    # RapidYaml
    if (TARGET ryml)
      set_target_properties(ryml PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
    endif()
    if (TARGET ryml-uninstall)
      set_target_properties(ryml-uninstall PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
    endif()
    if (TARGET c4core-amalgamate)
      set_target_properties(c4core-amalgamate PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
    endif()
    if (TARGET c4core)
      set_target_properties(c4core PROPERTIES FOLDER "Engine/Dependencies/RapidYaml")
    endif()

    #SDL2
    if (TARGET SDL_uclibc)
      set_target_properties(SDL_uclibc PROPERTIES FOLDER "Engine/Dependencies/SDL3")
    endif()
    if(TARGET SDL3-static)
      set_target_properties(SDL3-static PROPERTIES FOLDER "Engine/Dependencies/SDL3")
    endif()
    if(TARGET SDL3-shared)
      set_target_properties(SDL3-shared PROPERTIES FOLDER "Engine/Dependencies/SDL3")
    endif()
    if (TARGET sdl_headers_copy)
      set_target_properties(sdl_headers_copy PROPERTIES FOLDER "Engine/Dependencies/SDL2")
    endif()
    if (TARGET uninstall)
      set_target_properties(uninstall PROPERTIES FOLDER "Engine/Dependencies/SDL2")
    endif()

    #Vulkan
    if (TARGET asm_offset)
      set_target_properties(asm_offset PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET loader_specific_options)
      set_target_properties(loader_specific_options PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET loader-unknown-chain)
      set_target_properties(loader-unknown-chain PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET loader-opt)
            set_target_properties(loader-opt PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET Vulkan-Module)
            set_target_properties(Vulkan-Module PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET gpu_av_spirv)
            set_target_properties(gpu_av_spirv PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET loader_asm_gen_files)
            set_target_properties(loader_asm_gen_files PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR USE_VVL)
      if (TARGET vvl)
        set_target_properties(vvl PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
      endif()
      if (TARGET VkLayer_utils)
        set_target_properties(VkLayer_utils PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
      endif()
    endif()
    set_target_properties(vulkan PROPERTIES FOLDER "Engine/Dependencies/Vulkan")

    # Freetype
    set_target_properties(freetype PROPERTIES FOLDER "Engine/Dependencies")

    #Glslang
    if (TARGET glslang-standalone)
        set_target_properties(glslang-standalone PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET GenericCodeGen)
        set_target_properties(GenericCodeGen PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET glslang)
        set_target_properties(glslang PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET glslang-default-resource-limits)
        set_target_properties(glslang-default-resource-limits PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET MachineIndependent)
        set_target_properties(MachineIndependent PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET OSDependent)
        set_target_properties(OSDependent PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV)
        set_target_properties(SPIRV PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()

    #Imgui
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        set_target_properties(Imgui PROPERTIES FOLDER "Engine/Dependencies")
    endif()
endif()

if (USE_LTO)
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION ${USE_LTO})
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    VERSION "${PATACHE_ENGINE_VERSION}"
    CXX_EXTENSIONS OFF
	CXX_STANDARD 20)
