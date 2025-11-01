#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <iterator>

int main() {

    std::string words[] = {"new", "print", "if", "else", "fi", "loop", "done", "assign",
                           "arrow", "plus", "minus", "multiply",
                           "division", "less", "greater", "equal", "and", "or", "l_par", "r_par", "semicolon",
                           "ident", "number", "string", "end_of_file", "err"
    };

    std::ifstream programFile("../lang_idea/example.md");
    std::string program((std::istreambuf_iterator<char>(programFile)), std::istreambuf_iterator<char>());

    auto out = tokenize(program);
    for (int i = 0; i < out.size(); i++) std::cout << words[int(out[i].type)] << ": " << out[i].val << '\n';

    return 0;
}