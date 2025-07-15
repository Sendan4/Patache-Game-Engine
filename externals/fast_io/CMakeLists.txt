cmake_minimum_required(VERSION 3.18)

project("Fast_io" LANGUAGES CXX)

set(Fast_io_COMMIT_SHORT_HASH "788cb98" CACHE INTERNAL "Git Commit Short Hash")

add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE FAST_IO_OPTIMIZE_TIME)

message(STATUS "Header-Only Library")

target_include_directories(${PROJECT_NAME} INTERFACE "include/")

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

include(CheckCXXCompilerFlag)

if (NOT MSVC)
check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_C++23)
else()
check_cxx_compiler_flag("/std:c++23" COMPILER_SUPPORTS_C++23)
endif()

if (COMPILER_SUPPORTS_C++23)
	set (DEFINED_CXX_STANDARD 23)
else()
	if (NOT MSVC)
		check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_C++20)
	else()
		check_cxx_compiler_flag("/std:c++20" COMPILER_SUPPORTS_C++20)
	endif()

	if (COMPILER_SUPPORTS_C++20)
		set (DEFINED_CXX_STANDARD 20)
	else()
		message(ERROR "fast_io requires C++20")
	endif()

endif()

# Mingw in Msys2
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME} INTERFACE "-lntdll")
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_EXTENSIONS OFF
		CXX_STANDARD ${DEFINED_CXX_STANDARD})
