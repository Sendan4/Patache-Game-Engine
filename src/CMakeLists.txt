if (BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED)
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

add_subdirectory(RaccoonRenderer/Shaders)

add_dependencies(${PROJECT_NAME} PatacheFiles PatacheCompileShaders)

if (TARGET PatacheVulkanSDKFiles)
    add_dependencies(${PROJECT_NAME} PatacheVulkanSDKFiles)
endif()

if (NOT MSVC)
    if (${FORMAT_IN_BUILD_TIME})
        add_dependencies(${PROJECT_NAME} PatacheClangFormat)
    endif()
endif()

target_sources (${PROJECT_NAME} PRIVATE
    "Core/PatacheEngine.cpp"
    "Core/Configuration.cpp"
    "Core/Input.cpp"
    "Core/ClearColor.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_SetupRenderer.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_Swapchain.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_ImageView.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_CommandBuffer.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_FrameBuffer.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_DepthBuffer.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_SincronizationPrimitives.cpp"
    "RaccoonRenderer/Vulkan/Vulkan_Render.cpp"
    "RaccoonRenderer/Vulkan/VmaUsage.cpp"
    "RaccoonRenderer/BufferSubAllocation.cpp"
    "RaccoonRenderer/Draw.cpp"
    "Log/Message.cpp"
    "Log/StartLogInfo.cpp"
    "Log/Vulkan_SetupLog.cpp"
    "Log/Vulkan_Info.cpp")

if (MSVC)
    # List Headers Files in Visual Studio
    file(GLOB_RECURSE Include "${CMAKE_CURRENT_SOURCE_DIR}/../include/PatacheEngine/*.hpp")
    file(GLOB_RECURSE IncludeInternal "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

    target_sources(${PROJECT_NAME} PRIVATE ${Include} ${IncludeInternal})
endif()

# Windows resource file
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" OR WIN32)
    enable_language(RC)

    target_sources (${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/PatacheEngine.rc")
endif()

# Wayland impl for unix
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    enable_language(C)

    target_sources (${PROJECT_NAME} PRIVATE
        "Core/WaylandWindow.cpp")

    target_sources (${PROJECT_NAME} PRIVATE
				${CMAKE_CURRENT_SOURCE_DIR}/Core/WaylandProtocols/xdg-shell.c
				${CMAKE_CURRENT_SOURCE_DIR}/Core/WaylandProtocols/xdg-decoration-unstable-v1.c
				${CMAKE_CURRENT_SOURCE_DIR}/Core/WaylandProtocols/xdg-toplevel-icon-v1.c
				${CMAKE_CURRENT_SOURCE_DIR}/Core/WaylandProtocols/fractional-scale-v1.c)

    target_include_directories(${PROJECT_NAME} PRIVATE "Core/WaylandCSD/")

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Core/WaylandProtocols)
endif()

# ImGui
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_sources(${PROJECT_NAME} PRIVATE
        "ImGui/SetupImgui.cpp"
        "ImGui/DebugUI.cpp")

    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/ImGui/")
endif()

# Engine includes
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Log/"
    "${CMAKE_CURRENT_SOURCE_DIR}/RaccoonRenderer/Vulkan/")

# Compiler Flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GNU & Clang
    if (MINGW AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR
        ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo") AND COMPILER_SUPPORTS_FPERMISSIVE)
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

    # Disable MSVC C++ STL exceptions
    target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=0)
endif()

# Main engine macros
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

# Build info macros
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PATACHE_BUILD_SYSTEM="CMake"
    PATACHE_BUILD_SYSTEM_VERSION="${CMAKE_VERSION}"
    PATACHE_BUILD_SYSTEM_GENERATOR="${PATACHE_GENERATOR}"
    PATACHE_BUILD_SYSTEM_GENERATOR_VERSION="${PATACHE_GENERATOR_VERSION}"
    PATACHE_ENGINE_VERSION="${PATACHE_ENGINE_VERSION}"
    PATACHE_ENGINE_VERSION_MAYOR=${PATACHE_ENGINE_VERSION_MAYOR}
    PATACHE_ENGINE_VERSION_MINOR=${PATACHE_ENGINE_VERSION_MINOR}
    PATACHE_ENGINE_VERSION_PATCH=${PATACHE_ENGINE_VERSION_PATCH}
    PATACHE_ENGINE_NAME="Patache Engine"
    PATACHE_COMPILER_PROGRAM="${PATACHE_COMILER_PROGRAM}"
    PATACHE_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    PATACHE_ARCH="${CMAKE_SYSTEM_PROCESSOR}"
    PATACHE_FAST_IO_GIT_COMMIT_HASH_SHORT="${Fast_io_COMMIT_SHORT_HASH}"
    PATACHE_OS="${CMAKE_SYSTEM_NAME}"
    PATACHE_COMPILER="${CMAKE_CXX_COMPILER_ID}")

set(LINUX_OR_UNIX_MACROS_TEXT "__unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__ || __MidnightBSD__")

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(LINUX_OR_UNIX_MACROS_TEXT 0)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    VULKAN_HPP_NO_EXCEPTIONS
    VULKAN_HPP_NO_SMART_HANDLE
    VULKAN_HPP_NO_CONSTRUCTORS
    PATACHE_LINUX_OR_UNIX=${LINUX_OR_UNIX_MACROS_TEXT})

# Git info macros
if (USE_GIT AND GIT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PATACHE_USE_GIT
        PATACHE_GIT_BRANCH="${GIT_BRANCH}"
        PATACHE_GIT_HASH_LONG="${GIT_HASH_LONG}"
        PATACHE_GIT_HASH_SHORT="${GIT_HASH_SHORT}"
        PATACHE_GIT_DIRTY=${GIT_DIRTY}
        PATACHE_GIT_STAGE=${GIT_STAGE})
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_IMGUI_VERSION="${Imgui_VERSION}")
endif()

# Vulkan Validation Layer macro
if (USE_VVL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_USE_VVL)
endif()

# Link Libraries
# Wayland
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    if (${WAYLAND_FOUND} AND ${WAYLAND_CURSOR_FOUND})
        if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND CMAKE_C_COMPILER_ID STREQUAL "Clang")
            target_link_libraries(${PROJECT_NAME} PUBLIC
                -L/usr/local/lib
                -L/lib
                ${WAYLAND_LIBRARIES}
                ${WAYLAND_CURSOR_LIBRARIES})
        else()
            target_link_libraries(${PROJECT_NAME} PUBLIC
                ${WAYLAND_LIBRARIES}
                ${WAYLAND_CURSOR_LIBRARIES})
        endif()

        target_include_directories(${PROJECT_NAME} PUBLIC
            ${WAYLAND_INCLUDE_DIRS}
            ${WAYLAND_CURSOR_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "Wayland not found")
    endif()

    # Always define wayland macro for SDL
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_VIDEO_DRIVER_WAYLAND)
endif()

# Vulkan
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PATACHE_FIND_VVL_IN_THE_CURRENT_PATH)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly" OR
    ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    target_link_libraries(${PROJECT_NAME} PUBLIC vulkan)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"
    AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    target_link_libraries(${PROJECT_NAME} PUBLIC -L${VULKAN_RT_DLL_DIR} -lvulkan-1)
endif()

# Move this binaries to a build dir instead of bin build
if (TARGET asm_offset)
    set_target_properties(asm_offset
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC fast_io SDL3::SDL3 cglm GPUOpen::VulkanMemoryAllocator Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PRIVATE ryml::ryml freetype Vulkan::EnumStringHelper)
target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
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
    if (TARGET VulkanLayerSettings)
        set_target_properties(VulkanLayerSettings PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET VulkanSafeStruct)
        set_target_properties(VulkanSafeStruct PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (TARGET VulkanUtilityHeaders)
        set_target_properties(VulkanUtilityHeaders PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR USE_VVL)
        if (TARGET vvl)
            set_target_properties(vvl PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
        endif()
        if (TARGET VkLayer_utils)
            set_target_properties(VkLayer_utils PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
        endif()
        if (TARGET vvl_generated_setting_files)
            set_target_properties(vvl_generated_setting_files PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
        endif()
        if (TARGET vvl_gpu_av_shaders_spirv)
            set_target_properties(vvl_gpu_av_shaders_spirv PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
        endif()
    endif()

    if (TARGET vulkan)
        set_target_properties(vulkan PROPERTIES FOLDER "Engine/Dependencies/Vulkan")
    endif()

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
    if (TARGET spirv-tools-build-version)
        set_target_properties(spirv-tools-build-version PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-tools-header-DebugInfo)
        set_target_properties(spirv-tools-header-DebugInfo PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-tools-header-NonSemanticShaderDebugInfo100)
        set_target_properties(spirv-tools-header-NonSemanticShaderDebugInfo100 PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-tools-header-OpenCLDebugInfo100)
        set_target_properties(spirv-tools-header-OpenCLDebugInfo100 PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-cldi100)
        set_target_properties(spv-tools-cldi100 PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-clspvreflection)
        set_target_properties(spv-tools-clspvreflection PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-debuginfo)
        set_target_properties(spv-tools-debuginfo PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-shdi100)
        set_target_properties(spv-tools-shdi100 PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-spv-amd-gs)
        set_target_properties(spv-tools-spv-amd-gs PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-spv-amd-sb)
        set_target_properties(spv-tools-spv-amd-sb PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-spv-amd-sevp)
        set_target_properties(spv-tools-spv-amd-sevp PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-spv-amd-stm)
        set_target_properties(spv-tools-spv-amd-stm PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spv-tools-vkspreflection)
        set_target_properties(spv-tools-vkspreflection PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-tools-cpp-example)
        set_target_properties(spirv-tools-cpp-example PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-as)
        set_target_properties(spirv-as PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-cfg)
        set_target_properties(spirv-cfg PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-diff)
        set_target_properties(spirv-diff PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-dis)
        set_target_properties(spirv-dis PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-link)
        set_target_properties(spirv-link PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-lint)
        set_target_properties(spirv-lint PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-objdump)
        set_target_properties(spirv-objdump PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-opt)
        set_target_properties(spirv-opt PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-reduce)
        set_target_properties(spirv-reduce PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-val)
        set_target_properties(spirv-val PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-diff)
        set_target_properties(SPIRV-Tools-diff PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-link)
        set_target_properties(SPIRV-Tools-link PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-lint)
        set_target_properties(SPIRV-Tools-lint PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-opt)
        set_target_properties(SPIRV-Tools-opt PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-reduce)
        set_target_properties(SPIRV-Tools-reduce PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-shared)
        set_target_properties(SPIRV-Tools-shared PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET SPIRV-Tools-static)
        set_target_properties(SPIRV-Tools-static PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-tools-pkg-config)
        set_target_properties(spirv-tools-pkg-config PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET spirv-tools-vimsyntax)
        set_target_properties(spirv-tools-vimsyntax PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET core_tables)
        set_target_properties(core_tables PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET enum_string_mapping)
        set_target_properties(enum_string_mapping PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET extinst_tables)
        set_target_properties(extinst_tables PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET External)
        set_target_properties(External PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET Scripts)
        set_target_properties(Scripts PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()
    if (TARGET Shaders)
        set_target_properties(Shaders PROPERTIES FOLDER "Engine/Dependencies/Glslang")
    endif()

    #Imgui
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        set_target_properties(imgui PROPERTIES FOLDER "Engine/Dependencies")
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
