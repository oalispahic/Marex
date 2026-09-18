//
// Created by Omar Alispahic on 31. 10. 2025..
//
#ifndef MAREX_LEXER_HPP
#define MAREX_LEXER_HPP

#include <string>
#include <vector>
#include "token_.hpp"

class Lexer{
    std::vector<Token> Tokens;
    size_t curr = 0;
    int line = 1;
    std::string file;

public:
    explicit Lexer(const std::string &f) : file(f){}

    std::vector<Token> tokenize();

private:
    char peek();

    char current();

    size_t index();

    void consume();
};

#endif //MAREX_LEXER_HPP
