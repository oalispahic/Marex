//
// Created by Omar Alispahic on 31. 10. 2025..
//
#include <iostream>
#include <string>
#ifndef MAREX_LEXER_HPP
#define MAREX_LEXER_HPP

enum class TokenType{
    NEW = 0,
    PRINT,
    IF,
    ELSE,
    FI,
    LOOP,
    DONE,

    ASSIGN,
    ARROW,
    PLUS, MINUS, MULTIPLY, DIVISION,
    LESS, GREATER, EQUAL,
    AND, OR,
    L_PAR, R_PAR, SEMICOLON,

    IDENT,
    NUMBER,
    STRING,

    END_OF_FILE,
    ERR
};

struct Token{
    TokenType type;
    std::string val;
};

#endif //MAREX_LEXER_HPP
