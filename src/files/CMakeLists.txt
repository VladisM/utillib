cmake_minimum_required(VERSION 3.13.0)
project(utillib-files C)

set(inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
)

set(src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ihex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.c
)

set(target utillib-files)

set(want_libs
    utillib-core
)

add_library(${target} ${src})
target_link_libraries(${target} PRIVATE ${want_libs})
target_include_directories(${target} PUBLIC ${inc})
