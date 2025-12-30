//
// Created by Omar Alispahic on 31. 10. 2025..
//
#include <iostream>
#include <string>
#include <vector>
#include "token_.hpp"


#ifndef MAREX_TOKEN_HPP
#define MAREX_TOKEN_HPP



class Lexer{
    std::vector<Token> Tokens;
    int curr = 0;
    int line = 0;
    std::string file;

public:
    Lexer(const std::string &f) : file(f){}

private:

     char peek();

     char current();

     int index();

     void consume();

public:

    std::vector<Token> tokenize();
};


#endif //MAREX_TOKEN_HPP


