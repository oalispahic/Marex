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
    bool open_block_comment = false;

public:
    explicit Lexer(const std::string &f) : file(f){}

    std::vector<Token> tokenize();

    // True after tokenize() if the source ended inside a '/* ... */'
    // comment. The REPL uses it to keep reading lines.
    bool inOpenBlockComment() const { return open_block_comment; }

private:
    char peek();

    char current();

    size_t index();

    void consume();
};

#endif //MAREX_LEXER_HPP
