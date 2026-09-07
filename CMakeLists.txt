cmake_minimum_required(VERSION 3.5)
project(expressionparser)

set(CMAKE_CXX_STANDARD 20)

add_executable(expressionparser main.cpp
        parser.h
        parser.cpp)
