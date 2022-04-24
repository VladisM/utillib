cmake_minimum_required(VERSION 3.13.0)
project(utillib C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-g -O0)
    IF(UNIX)
        add_link_options(-rdynamic)
        add_definitions(-DENABLE_CALL_STACK_DUMP_AT_ERROR)
    ENDIF()
ENDIF()

set(CMAKE_C_STANDARD 99)
set(BUILD_STATIC_LIBS ON)

set(include_files ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_compile_options(-Wall -Wextra)

add_library(utillib-core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/dynmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/array.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/atexit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/string.c
)
add_library(utillib-cli
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/options.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/question.c
)
add_library(utillib-utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/tokenizer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/error_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/evaluate.c
)
add_library(utillib-files
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/ihex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/mif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/common.c
)

target_link_libraries(utillib-cli PRIVATE utillib-core)
target_link_libraries(utillib-cli PRIVATE utillib-utils)
target_link_libraries(utillib-utils PRIVATE utillib-core m)
target_link_libraries(utillib-files PRIVATE utillib-core)

target_include_directories(utillib-core PUBLIC ${include_files})
target_include_directories(utillib-cli PUBLIC ${include_files})
target_include_directories(utillib-utils PUBLIC ${include_files})
target_include_directories(utillib-files PUBLIC ${include_files})
