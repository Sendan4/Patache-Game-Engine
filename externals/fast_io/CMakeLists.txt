cmake_minimum_required(VERSION 3.18)

message("")
message("  - Fast_io -")

project("Fast_io" LANGUAGES CXX)

set(Fast_io_COMMIT_SHORT_HASH "804d943" CACHE INTERNAL "Git Commit Short Hash")

add_library(${PROJECT_NAME} INTERFACE)

message(STATUS "Header-Only Library")

target_include_directories(${PROJECT_NAME} INTERFACE "include/")

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
