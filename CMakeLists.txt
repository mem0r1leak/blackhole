cmake_minimum_required(VERSION 4.1)
project(blackhole)

set(CMAKE_CXX_STANDARD 23)

add_executable(blackhole main.cpp
        terminal.h
        huffman.cpp
        huffman.h)
