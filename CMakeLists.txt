cmake_minimum_required(VERSION 3.8)
project(3dsdl)

include_directories(src)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SRC src/*.cpp)

link_libraries(SDL2 SDL2main)

add_executable(3dsdl ${SRC})
