//
// Created by Omar Alispahic on 31. 10. 2025..
//
#ifndef MAREX_TOKEN_HPP
#define MAREX_TOKEN_HPP

#include <string>

enum class TokenType {
    VAR = 0,
    PRINT,
    IF,
    ELSE,
    FI,
    LOOP,
    DONE,
    STEP,
    FUN,
    SYS,

    ASSIGN,
    ARROW,
    PLUS, PLUSPLUS, MINUS, MULTIPLY, DIVISION,
    LESS, GREATER, EQUAL, NOT_EQ,
    AND, OR,
    L_PAR, R_PAR, L_BRACKET, R_BRACKET, DOT, SEMICOLON, NEWLN,

    IDENT,
    NUMBER,
    FLOAT,
    STRING,

    END_OF_FILE,
    ERR,
};

struct Token {
    TokenType type;
    std::string val;
    int token_line;   // 1-based source line the token starts on
};

#endif //MAREX_TOKEN_HPP
