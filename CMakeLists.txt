cmake_minimum_required(VERSION 3.28)
project(Marex)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Lexer)

add_executable(Marex
        Lexer/token_.hpp
        Lexer/lexer.hpp
        Lexer/lexer.cpp
        ast/ast_nodes.hpp
        ast/ast_nodes.hpp
        parser/parser.hpp
        parser/parser.cpp
        interpreter/interpreter.hpp
        interpreter/interpreter.cpp
        src/testing.cpp)

