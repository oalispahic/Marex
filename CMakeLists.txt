cmake_minimum_required(VERSION 3.28)
project(Marex)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Lexer)

add_executable(Marex
        include/token_.hpp
        include/lexer.hpp
        include/ast_nodes.hpp
        include/parser.hpp

        include/interpreter.hpp
        src/parser.cpp
        src/lexer.cpp
        src/interpreter.cpp
        src/testing.cpp)

