cmake_minimum_required(VERSION 3.13.0)
project(utillib-utils C)

set(inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

set(src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/error_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/evaluate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string_cache.c
)

set(target utillib-utils)

set(want_libs
    utillib-core
    m
)

add_library(${target} ${src})
target_link_libraries(${target} PRIVATE ${want_libs})
target_include_directories(${target} PUBLIC ${inc})
