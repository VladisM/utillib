cmake_minimum_required(VERSION 3.13.0)
project(utillib-core C)

option(ENABLE_CALL_STACK_DUMP "Enable dump of call stack on errors. Reqire debug configuration and UNIX host." ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-g -O0)
    IF(UNIX AND ENABLE_CALL_STACK_DUMP)
        add_link_options(-rdynamic)
        add_definitions(-DENABLE_CALL_STACK_DUMP_AT_ERROR)
    ENDIF()
ENDIF()

set(inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

set(src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dynmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/array.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/atexit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.c
)

set(target utillib-core)

add_library(${target} ${src})
target_include_directories(${target} PUBLIC ${inc})
