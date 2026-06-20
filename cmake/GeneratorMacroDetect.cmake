function(GeneratorMacroDetect PATACHE_GENERATOR PATACHE_GENERATOR_VERSION)
    message(STATUS "DETECTING GENERATOR INFO")

    set(_generator_version "")

    if(CMAKE_MAKE_PROGRAM)
        execute_process(
            COMMAND "${CMAKE_MAKE_PROGRAM}" --version
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT_VARIABLE _raw_output
            ERROR_VARIABLE _raw_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE
        )

        if(NOT "${_raw_output}" STREQUAL "")
            if("${CMAKE_GENERATOR}" MATCHES "Ninja")
                set(_generator_version "${_raw_output}")
            else()
                string(REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?" _generator_version "${_raw_output}")
            endif()
        endif()
    endif()

    if("${_generator_version}" STREQUAL "")
        set(_generator_version "UNKNOWN")
    endif()

    set(${PATACHE_GENERATOR}         "${CMAKE_GENERATOR}"     PARENT_SCOPE)
    set(${PATACHE_GENERATOR_VERSION} "${_generator_version}" PARENT_SCOPE)

    message(STATUS "GET STRING PATACHE_GENERATOR : ${CMAKE_GENERATOR}")
    message(STATUS "GET STRING PATACHE_GENERATOR_VERSION : ${_generator_version}")
endfunction()
