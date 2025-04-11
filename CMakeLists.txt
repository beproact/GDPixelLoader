cmake_minimum_required(VERSION 3.30)
project(PixelLoader)

set(CMAKE_CXX_STANDARD 20)

add_executable(PixelLoader PixelLoader.cpp
        pixelLoader.hpp
        main.cpp)
