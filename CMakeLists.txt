cmake_minimum_required(VERSION 3.13.0)
project(utillib C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 99)
set(BUILD_STATIC_LIBS ON)

add_compile_options(-Wall -Wextra)

add_subdirectory(${CMAKE_CURRENT_SOURCE_FILE}src/cli)
add_subdirectory(${CMAKE_CURRENT_SOURCE_FILE}src/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_FILE}src/files)
add_subdirectory(${CMAKE_CURRENT_SOURCE_FILE}src/utils)
