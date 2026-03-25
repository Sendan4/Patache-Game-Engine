cmake_minimum_required(VERSION 3.18)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
    set (CMAKE_SYSTEM_VERSION 6.1.7601) # Windows 7
endif()

message("")
message(" - MultipleTriangles -")

project("MultipleTriangles" LANGUAGES CXX)

if (${PATACHE_USE_BIN_DIR})
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PATACHE_OUTPUT_DIR})
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PATACHE_OUTPUT_DIR})
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(DEFAULT_USE_LTO_OPT ON)
else()
    set(DEFAULT_USE_LTO_OPT OFF)
endif()
    
option(USE_LTO "Use Link Time Optimization" ${DEFAULT_USE_LTO_OPT})

if (${CMAKE_BUILD_TYPE} STREQUAL "Release" AND USE_LTO)
    include(CheckIPOSupported)
    
    check_ipo_supported(
  	RESULT COMPILER_SUPPORT_LTO
  	LANGUAGES CXX
    )
    
    if (NOT COMPILER_SUPPORT_LTO)
        set(USE_LTO OFF)
        message(STATUS "${CMAKE_CXX_COMPILER_ID} no support LTO")
    endif()
    
    # LTO dont work well with clang in static builds
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND NOT BUILD_SHARED_LIBS)
        set(USE_LTO OFF) 
    endif()
endif()

add_executable(${PROJECT_NAME} "MultipleTriangles.cpp")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set_target_properties(
        ${PROJECT_NAME} 
        PROPERTIES 
            OUTPUT_NAME "${PROJECT_NAME}.${CMAKE_HOST_SYSTEM_PROCESSOR}"
            SUFFIX ".elf"
    )
endif()

# Windows resource file
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" OR WIN32)
    enable_language(RC)

    target_sources (${PROJECT_NAME} PRIVATE "MultipleTriangles.rc")
endif()

if (TARGET ClangFormat AND FORMAT_IN_BUILD_TIME)
    add_dependencies(${PROJECT_NAME} ClangFormat)
endif()

if (NOT MSVC)
    # GCC
    check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_C++20) # Obligatory
    check_cxx_compiler_flag("-Wextra" COMPILER_SUPPORTS_WEXTRA)
    check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
    check_cxx_compiler_flag("-Wpedantic" COMPILER_SUPPORTS_WPEDANTIC)
    check_cxx_compiler_flag("-fno-exceptions" COMPILER_SUPPORTS_NO_EXCEPTIONS) # Obligatory
    check_cxx_compiler_flag("-fno-strict-aliasing" COMPILER_SUPPORTS_NO_STRICT_ALIASING)
    check_cxx_compiler_flag("-ffast-math" COMPILER_SUPPORTS_FAST_MATH) # Obligatory

    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        check_cxx_compiler_flag("-mmmx" COMPILER_SUPPORTS_MMX) # Obligatory
        check_cxx_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE4.2) # Obligatory
        check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX) # Obligatory
    endif()

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
        check_cxx_compiler_flag("-fno-rtti" COMPILER_SUPPORTS_NO_RTTI) # Obligatory

        if (NOT COMPILER_SUPPORTS_NO_RTTI) # Obligatory
            message(FATAL_ERROR "The compiler does not support -fno-rtti")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti)
        endif()
    endif()

    if (NOT COMPILER_SUPPORTS_C++20) # Obligatory
        message(FATAL_ERROR "The compiler does not support the C++20 standard")
    endif()

    if (NOT COMPILER_SUPPORTS_NO_EXCEPTIONS) # Obligatory
        message(FATAL_ERROR "Exceptions cannot be disabled in this compiler")
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -fno-exceptions)
    endif()

    if (NOT COMPILER_SUPPORTS_FAST_MATH) # Obligatory
        message(FATAL_ERROR "The compiler does not support -ffast-math")
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -ffast-math)
    endif()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        if (NOT COMPILER_SUPPORTS_SSE4.2) # Obligatory
            message(FATAL_ERROR "The compiler does not support -msse4.2")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE -msse4.2)
        endif()

        if (NOT COMPILER_SUPPORTS_MMX) # Obligatory
            message(FATAL_ERROR "The compiler does not support -mmmx")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE -mmmx)
        endif()

        if (NOT COMPILER_SUPPORTS_AVX) # Obligatory
            message(FATAL_ERROR "The compiler does not support -mavx")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE -mavx)
        endif()
    endif()
else()
    # MSVC
    check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_C++20) # Obligatory
    check_cxx_compiler_flag("/W4" COMPILER_SUPPORTS_W4)
    check_cxx_compiler_flag("/fp:fast" COMPILER_SUPPORTS_FLOATING_POINT_FAST) # Obligatory
    check_cxx_compiler_flag("/fp:except-" COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT) # Obligatory

    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        check_cxx_compiler_flag("/arch:SSE4.2" COMPILER_SUPPORTS_SSE4.2) # Obligatory
        check_cxx_compiler_flag("/arch:AVX" COMPILER_SUPPORTS_AVX) # Obligatory
    endif()

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
        check_cxx_compiler_flag("/GR-" COMPILER_SUPPORTS_NO_RTTI) # Obligatory

        if (NOT COMPILER_SUPPORTS_NO_RTTI) # Obligatory
            message(FATAL_ERROR "The compiler does not support /GR-")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE /GR-)
        endif()
    endif()

    if (NOT COMPILER_SUPPORTS_C++20) # Obligatory
        message(FATAL_ERROR "The compiler does not support the C++20 standard")
    endif()
    
    if (COMPILER_SUPPORTS_W4)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    endif()

    if (NOT COMPILER_SUPPORTS_FLOATING_POINT_FAST) # Obligatory
        message(FATAL_ERROR "The compiler does not support /fp:fast")
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast)
    endif()

    if (NOT COMPILER_SUPPORTS_NO_EXCEPCTION_FLOATING_POINT) # Obligatory
        message(FATAL_ERROR "The compiler cannot disable excepctions for floating point")
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE /fp:except-)
    endif()

    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        if (NOT COMPILER_SUPPORTS_SSE4.2) # Obligatory
            message(FATAL_ERROR "The compiler does not support /arch:SSE4.2")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE /arch:SSE4.2)
        endif()
    
        if (NOT COMPILER_SUPPORTS_AVX) # Obligatory
            message(FATAL_ERROR "The compiler does not support /arch:AVX")
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX)
        endif()
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE PatacheEngine)

if (USE_LTO)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION ${USE_LTO})
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20)
