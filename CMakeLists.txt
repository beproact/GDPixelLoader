cmake_minimum_required(VERSION 3.30)
project(PixelLoader)

set(CMAKE_CXX_STANDARD 20)

add_executable(PixelLoader gdPixelLoader.cpp
        gdPixelLoader.hpp
        main.cpp)

target_include_directories(PixelLoader PRIVATE include)