# Compile GLSL to SPIR-V
add_custom_target(PatataCompileShaders)

if (MSVC)
    set (OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/Shaders")
else()
    set (OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders")
endif()

add_custom_command(
  POST_BUILD
  TARGET PatataCompileShaders
  COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR})

file(GLOB ShaderFiles "*.vert" "*.frag")

# Build Shaders
foreach(SHADER ${ShaderFiles})
  get_filename_component(ShaderFileName ${SHADER} NAME)

  add_custom_command(
    POST_BUILD
    TARGET PatataCompileShaders
    COMMAND $<TARGET_FILE:glslang-standalone> ${SHADER} -o ${OUTPUT_DIR}/${ShaderFileName}.spv -V)
endforeach()

