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
    for (auto & i : out) std::cout << words[int(i.type)] << ": " << i.val << '\n';

    return 0;
}