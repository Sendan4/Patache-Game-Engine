# Compile GLSL to SPIR-V
add_custom_target(PatacheCompileShaders)

if (MSVC)
    set (OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/Shaders")
else()
    set (OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders")
endif()

add_custom_command(
    POST_BUILD
    TARGET PatacheCompileShaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR})

file(GLOB ShaderFiles "*.vert" "*.frag")

list(LENGTH ShaderFiles ShaderCount)

set(ShaderIterator 1)

if (IS_READABLE ${VULKAN_SDK_GLSLANG_EXECUTABLE})
    set(GLSLANG ${VULKAN_SDK_GLSLANG_EXECUTABLE})
else()
    set(GLSLANG $<TARGET_FILE:glslang-standalone>)
endif()

# Build Shaders
foreach(SHADER ${ShaderFiles})
    get_filename_component(ShaderFileName ${SHADER} NAME)

    add_custom_command(
        POST_BUILD
        TARGET PatacheCompileShaders
				COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "[${ShaderIterator}/${ShaderCount}] Building SPIR-V object ${ShaderFileName}.spv"
        COMMAND ${GLSLANG} ${SHADER} -o ${OUTPUT_DIR}/${ShaderFileName}.spv -V -lto --quiet)

	math(EXPR ShaderIterator "${ShaderIterator} + 1" OUTPUT_FORMAT DECIMAL)
endforeach()

