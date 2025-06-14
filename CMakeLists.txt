cmake_minimum_required(VERSION 3.31)
project(terminal_tetris)

set(CMAKE_CXX_STANDARD 20)

add_executable(terminal_tetris
        tetris.cpp
        tetris.h
        terminal_helpers.cpp
        terminal_helpers.h)
