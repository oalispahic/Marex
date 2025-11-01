cmake_minimum_required(VERSION 3.28)
project(Marex)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Lexer)

add_executable(Marex
        Lexer/lexer.hpp
        Lexer/token.hpp)
