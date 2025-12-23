//
// Created by Omar Alispahic on 23. 12. 2025..
//
#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <iterator>
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"


int main() {

    std::string words[] = {"new", "print", "if", "else", "fi", "loop", "done", "assign",
                           "arrow", "plus", "minus", "multiply",
                           "division", "less", "greater", "equal","not_equal", "and", "or", "l_par", "r_par", "semicolon","newln",
                           "ident", "number", "string", "end_of_file", "err",
    };

    std::ifstream programFile("../lang_idea/example.md");
    std::string program((std::istreambuf_iterator<char>(programFile)), std::istreambuf_iterator<char>());

    Lexer lex(program);
    auto out = lex.tokenize();
    for (auto & i : out) std::cout << words[int(i.type)] << ": " << i.val << '\n';

    Parser pars(out);
    Program * parsed;
    bool failed = false;
    try{
        parsed = pars.parse();
    }catch(std::runtime_error &err){
        std::cout<<err.what();
        failed = true;
    }
    if(failed) return 0;

    Interpreter new_int;
    new_int.run(parsed);

    return 0;
}