cmake_minimum_required(VERSION 3.26)
project(lab1_1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1_1 main.cpp
        circular_buffer/circular_buffer.cpp
        circular_buffer/circular_buffer.h)
