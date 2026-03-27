cmake_minimum_required(VERSION 3.20)
project(sorting_visualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(sorting_visualizer src/main.cpp)

target_link_libraries(sorting_visualizer PRIVATE SDL3::SDL3)
