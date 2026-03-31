cmake_minimum_required(VERSION 3.28)
project(Marex VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated/version.hpp
        @ONLY
)

include_directories(.)
include_directories(Lexer)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)

add_executable(Marex
        include/token_.hpp
        include/lexer.hpp
        include/ast_nodes.hpp
        include/parser.hpp

        include/interpreter.hpp
        include/cli.hpp
        src/parser.cpp
        src/lexer.cpp
        src/interpreter.cpp
        src/cli.cpp
        test_program/testing.cpp
        src/main.cpp
        include/values_types.hpp
        src/values_types.cpp
)
