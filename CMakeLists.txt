cmake_minimum_required(VERSION 3.10.0)
project(myalloc VERSION 0.1.0 LANGUAGES C)

add_executable(heap main.c)

add_subdirectory(myalloc)

target_link_libraries(heap PRIVATE myalloc)
