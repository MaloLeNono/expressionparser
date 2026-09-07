cmake_minimum_required(VERSION 3.5)
project(expressionparser)

set(CMAKE_CXX_STANDARD 20)

add_executable(expressionparser src/main.cpp
        src/parser.h
        src/parser.cpp)
