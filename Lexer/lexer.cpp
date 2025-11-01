#include "lexer.hpp"
#include <iostream>


int main() {

    std::string words[] = {"new", "print", "if", "else", "fi", "loop", "done", "assign",
                           "arrow", "plus", "minus", "multiply",
                           "division", "less", "greater", "equal", "and", "or", "l_par", "r_par", "semicolon",
                           "ident", "number", "string", "end_of_file", "err"
    };

    std::string test = R"(
new test_var := 5
new another_var := 10
if (test_var < another_var)
    print("Hey this works!")
fi
print("Hello World")
new a := 5
loop(a -> another_var)
    print("This should print 4 times")
done
)";
    auto out = tokenize(test);

    for (int i = 0; i < out.size(); i++) std::cout << words[int(out[i].type)] << ": " << out[i].val << '\n';

    return 0;
}