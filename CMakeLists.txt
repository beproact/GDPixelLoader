cmake_minimum_required(VERSION 3.30)
project(PixelLoader)

set(CMAKE_CXX_STANDARD 20)

add_executable(PixelLoader gdloader/gdPixelLoader.cpp
        gdloader/gdPixelLoader.hpp
        main.cpp)

target_include_directories(PixelLoader PRIVATE include)
target_include_directories(PixelLoader PRIVATE gdloader)