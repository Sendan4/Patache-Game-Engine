find_program(CLANGFORMAT NAMES clang-format OPTIONAL)

if (NOT ${CLANGFORMAT} STREQUAL "")
    message("")
    message(STATUS "clang-format Found : ${CLANGFORMAT}")

    add_custom_target(PatacheClangFormat)

    add_custom_command(
        POST_BUILD
        TARGET PatacheClangFormat
        COMMAND ${CLANGFORMAT}
            -i ${CMAKE_CURRENT_SOURCE_DIR}/include/PatacheEngine/*.hpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.hpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/*.cpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/*.hpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/Log/*.cpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/Log/*.hpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/RaccoonRenderer/*.cpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/RaccoonRenderer/Vulkan/*.cpp
            -i ${CMAKE_CURRENT_SOURCE_DIR}/src/RaccoonRenderer/Vulkan/*.hpp
            --verbose)

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR
        ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR
        ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR
        ${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly" OR
        ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
        add_custom_command(
           POST_BUILD
           TARGET PatacheClangFormat
           COMMAND ${CLANGFORMAT}
               -i ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/WaylandCSD/*.hpp
               --verbose)
    endif()
    
    if (${BUILD_EXAMPLES})
        add_custom_command(
           POST_BUILD
           TARGET PatacheClangFormat
           COMMAND ${CLANGFORMAT}
               -i ${CMAKE_CURRENT_SOURCE_DIR}/examples/Empty/*.cpp
               -i ${CMAKE_CURRENT_SOURCE_DIR}/examples/Triangle/*.cpp
               -i ${CMAKE_CURRENT_SOURCE_DIR}/examples/MultipleTriangles/*.cpp
               --verbose)
    endif()
else()
  	message(WARNING "clang-format not found")
endif()
