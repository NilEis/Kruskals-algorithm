cmake_minimum_required(VERSION 3.0.0)
project(maze_gen VERSION 0.1.0)

add_subdirectory(extern/union_find)

add_executable(maze_gen src/main.c)

target_link_libraries(maze_gen union_find)
