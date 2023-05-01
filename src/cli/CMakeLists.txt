cmake_minimum_required(VERSION 3.13.0)
project(utillib-cli C)

set(inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

set(src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/options.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/question.c
)

set(target utillib-cli)

set(want_libs
    utillib-core
    utillib-utils
)

add_library(${target} ${src})
target_link_libraries(${target} PRIVATE ${want_libs})
target_include_directories(${target} PUBLIC ${inc})
