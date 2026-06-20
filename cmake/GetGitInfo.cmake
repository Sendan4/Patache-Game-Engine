function(GetGitInfo GIT_BRANCH GIT_HASH_LONG GIT_HASH_SHORT GIT_DIRTY GIT_STAGE)
    find_program(GIT NAMES git)

    # Initialize local variables
    set(_branch "")
    set(_hash_long "")
    set(_hash_short "")
    set(_dirty 0)
    set(_stage 0)

    if (USE_GIT AND GIT)
        message(STATUS "EXECUTABLE git Found : ${GIT}")
        message(STATUS "DETECTING GIT INFO")

        # 1. Get Branch Name
        execute_process(
            COMMAND ${GIT} branch --show-current
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE _branch
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        message(STATUS "GET STRING GIT_BRANCH : ${_branch}")

        # 2. Get Long Hash
        execute_process(
            COMMAND ${GIT} rev-parse HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE _hash_long
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        message(STATUS "GET STRING GIT_HASH_LONG : ${_hash_long}")

        # 3. Get Short Hash
        execute_process(
            COMMAND ${GIT} rev-parse --short HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE _hash_short
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        message(STATUS "GET STRING GIT_HASH_SHORT : ${_hash_short}")

        # 4. Check for Unstaged/Staged changes (Dirty)
        execute_process(
            COMMAND ${GIT} diff HEAD --shortstat
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE PATACHE_GIT_IS_DIRTY
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        if (NOT "${PATACHE_GIT_IS_DIRTY}" STREQUAL "")
            set(_dirty 1)
        endif()

        message(STATUS "GET BOOLEAN GIT_DIRTY : ${_dirty}")

        # 5. Check for Staged changes only
        execute_process(
            COMMAND ${GIT} diff HEAD --shortstat --staged
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE PATACHE_GIT_COMMIT_IS_IN_STAGE
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        if (NOT "${PATACHE_GIT_COMMIT_IS_IN_STAGE}" STREQUAL "")
            set(_stage 1)
        endif()

        message(STATUS "GET BOOLEAN GIT_STAGE : ${_stage}")
    else()
        message(WARNING "EXECUTABLE git not found or USE_GIT is disabled")
    endif()

    # CRITICAL: Elevate local variables to the caller's scope
    set(${GIT_BRANCH}     "${_branch}"     PARENT_SCOPE)
    set(${GIT_HASH_LONG}  "${_hash_long}"  PARENT_SCOPE)
    set(${GIT_HASH_SHORT} "${_hash_short}" PARENT_SCOPE)
    set(${GIT_DIRTY}      "${_dirty}"      PARENT_SCOPE)
    set(${GIT_STAGE}      "${_stage}"      PARENT_SCOPE)
endfunction()
