if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"
    AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    message("")
    message("  - Vulkan SDK -")

    set(VULKAN_SDK_INSTALL_DIR "${CMAKE_BINARY_DIR}/vulkansdk")
    set(VULKAN_SDK_INSTALLER_DIR "${CMAKE_BINARY_DIR}/vulkansdk_installer.exe")

    if (NOT EXISTS ${VULKAN_SDK_INSTALL_DIR})
        file(MAKE_DIRECTORY "${VULKAN_SDK_INSTALL_DIR}")

        if (NOT EXISTS ${VULKAN_SDK_INSTALL_DIR})
            message(FATAL_ERROR "Fail to create a vulkan sdk install dir")
        endif()

        message(STATUS "DOWNLOADING Vulkan SDK")

        file(DOWNLOAD
            "https://sdk.lunarg.com/sdk/download/1.4.350.0/windows/vulkansdk-windows-X64-1.4.350.0.exe"
            "${VULKAN_SDK_INSTALLER_DIR}"
            SHOW_PROGRESS)
    else()
        message(STATUS "ALREADY DOWNLOADED")
    endif()

    if (NOT EXISTS "${VULKAN_SDK_INSTALL_DIR}/Bin")
        if (NOT IS_READABLE "${VULKAN_SDK_INSTALLER_DIR}")
            message(FATAL_ERROR "Vulkan SDK Installer Not Found")
        endif()

        message(STATUS "INSTALLING Vulkan SDK")

        file(ARCHIVE_EXTRACT
            INPUT ${VULKAN_SDK_INSTALLER_DIR}
            DESTINATION ${VULKAN_SDK_INSTALL_DIR}
            VERBOSE)
    else()
        message(STATUS "ALREADY INSTALLED")
    endif()

    if (IS_READABLE ${VULKAN_SDK_INSTALLER_DIR})
        message(STATUS "REMOVE INSTALLER Vulkan SDK : ${VULKAN_SDK_INSTALLER_DIR}")
        file(REMOVE ${VULKAN_SDK_INSTALLER_DIR})
        set (VULKAN_SDK_INSTALLER_DIR "")
    endif()

    # GLSLANG
    set(VULKAN_SDK_GLSLANG_EXECUTABLE "${VULKAN_SDK_INSTALL_DIR}/Bin/glslang.exe" CACHE INTERNAL "..." FORCE)

    if (NOT EXISTS ${VULKAN_SDK_GLSLANG_EXECUTABLE})
        message(STATUS "Cannot find glslang executable")
    endif()

    message(STATUS "EXECUTABLE glslang Found : ${VULKAN_SDK_GLSLANG_EXECUTABLE}")

    # VVL
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
        set(VULKAN_SDK_VVL_DLL "${VULKAN_SDK_INSTALL_DIR}/Bin/VkLayer_khronos_validation.dll" CACHE INTERNAL "..." FORCE)
        set(VULKAN_SDK_VVL_DLL_DIR "${VULKAN_SDK_INSTALL_DIR}/Bin/" CACHE INTERNAL "..." FORCE)

        if (NOT EXISTS ${VULKAN_SDK_VVL_DLL})
            message(FATAL_ERROR "Cannot find VkLayer_khronos_validation dinamic link lib")
        endif()

        message(STATUS "DINAMIC LINK LIB VkLayer_khronos_validation Found : ${VULKAN_SDK_VVL_DLL}")

        set(VULKAN_SDK_VVL_JSON "${VULKAN_SDK_INSTALL_DIR}/Bin/VkLayer_khronos_validation.json" CACHE INTERNAL "..." FORCE)

        if (NOT EXISTS ${VULKAN_SDK_VVL_JSON})
            message(FATAL_ERROR "Cannot find VkLayer_khronos_validation json")
        endif()

        message(STATUS "JSON VkLayer_khronos_validation Found : ${VULKAN_SDK_VVL_JSON}")
    endif()

    # Vulkan RunTime
    message("")
    message("  - Vulkan RunTime -")

    set(VULKAN_RT_INSTALL_DIR "${CMAKE_BINARY_DIR}/vulkanrt")
    set(VULKAN_RT_INSTALLER_DIR "${CMAKE_BINARY_DIR}/vulkanruntime.zip")

    if (NOT EXISTS ${VULKAN_RT_INSTALL_DIR})
        message(STATUS "DOWNLOADING Vulkan RunTime")
        file(MAKE_DIRECTORY "${VULKAN_RT_INSTALL_DIR}")

        if (NOT EXISTS ${VULKAN_RT_INSTALL_DIR})
            message(FATAL_ERROR "Cannot create a vulkan RunTime installation dir")
        endif()

        file(DOWNLOAD
            "https://sdk.lunarg.com/sdk/download/1.4.350.0/windows/VulkanRT-X64-1.4.350.0-Components.zip"
            "${VULKAN_RT_INSTALLER_DIR}"
            SHOW_PROGRESS)

        if (NOT EXISTS ${VULKAN_RT_INSTALLER_DIR})
            message(FATAL_ERROR "Vulkan RunTime Not Found")
        endif()
    else()
        message(STATUS "ALREADY DOWNLOADED")
    endif()

    if (NOT EXISTS "${VULKAN_RT_INSTALL_DIR}/VulkanRT-X64-1.4.350.0-Components/x64")
        message(STATUS "INSTALLING Vulkan RunTime")

        file(ARCHIVE_EXTRACT
            INPUT ${VULKAN_RT_INSTALLER_DIR}
            DESTINATION ${VULKAN_RT_INSTALL_DIR}
            VERBOSE)
    else()
        message(STATUS "ALREADY INSTALLED")
    endif()

    if (IS_READABLE ${VULKAN_RT_INSTALLER_DIR})
        message(STATUS "REMOVE ZIP Vulkan RunTime : ${VULKAN_RT_INSTALLER_DIR}")
        file(REMOVE ${VULKAN_RT_INSTALLER_DIR})
        set (VULKAN_RT_INSTALLER_DIR "")
    endif()

    set(VULKAN_RT_DLL "${VULKAN_RT_INSTALL_DIR}/VulkanRT-X64-1.4.350.0-Components/x64/vulkan-1.dll" CACHE INTERNAL "..." FORCE)
    set(VULKAN_RT_DLL_DIR "${VULKAN_RT_INSTALL_DIR}/VulkanRT-X64-1.4.350.0-Components/x64/" CACHE INTERNAL "..." FORCE)

    if (NOT EXISTS ${VULKAN_RT_DLL})
        message(FATAL_ERROR "Cannot find vulkan-1 executable")
    endif()

    message(STATUS "DINAMIC LINK LIB vulkan-1 Found : ${VULKAN_RT_DLL}")

    # Install files
    add_custom_target(PatacheVulkanSDKFiles
        COMMENT "Copying Vulkan SDK Files")

    if (MSVC)
        set_target_properties(PatacheVulkanSDKFiles PROPERTIES FOLDER "Engine")
    endif()

    get_filename_component(VULKAN_RT_DLL_NAME ${VULKAN_RT_DLL} NAME)

    add_custom_command(
        POST_BUILD
        TARGET PatacheVulkanSDKFiles
		COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "Copying DINAMIC LINK LIB ${VULKAN_RT_DLL_NAME}"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${VULKAN_RT_DLL}
            ${BIN_DIR})

    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND USE_VVL)
        get_filename_component(VULKAN_SDK_VVL_DLL_NAME ${VULKAN_SDK_VVL_DLL} NAME)
        get_filename_component(VULKAN_SDK_VVL_JSON_NAME ${VULKAN_SDK_VVL_JSON} NAME)

        add_custom_command(
            POST_BUILD
            TARGET PatacheVulkanSDKFiles
		    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "Copying DINAMIC LINK LIB ${VULKAN_SDK_VVL_DLL_NAME}"
            COMMAND ${CMAKE_COMMAND} -E copy
                ${VULKAN_SDK_VVL_DLL}
                ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "Copying JSON ${VULKAN_SDK_VVL_JSON_NAME}"
            COMMAND ${CMAKE_COMMAND} -E copy
                ${VULKAN_SDK_VVL_JSON}
                ${BIN_DIR})
     endif()
endif()