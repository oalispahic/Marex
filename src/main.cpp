//
// Created by Omar Alispahic on 23. 12. 2025..
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"


void clear_terminal(){
#if defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
    system("cls");
#else
    system("clear");
#endif
}
void compile_and_run(const std::string &source){
    try{
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto program = parser.parse();

        Interpreter interpreter;
        interpreter.run(program);
    }
    catch(const std::exception &e){
        std::cerr<<e.what()<<'\n';
        return;
    }
}

void repl(){
    std::string line;
    std::string program;
    clear_terminal();
    std::cout<<"Welcome to Marex REPL (type :run to execute, :exit to quit)\n";

    while(true){
        std::cout<<"marex> ";
        std::getline(std::cin,line);

        if(line==":exit"){
            std::cout<<'\n'<<"Quit!"<<'\n';
            break;
        }
        if(line == ":run"){
            compile_and_run(program);
            std::cout<<'\n';
            program.clear();
            continue;
        }
        program += line;
    }
}


int main(int argc, char** argv){

    if(argc != 2){
        repl();
        return 1;
    }

    std::ifstream file(argv[1]);
    if(!file){
        std::cerr<<"Error opening file "<<argv[1]<<'\n';
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    compile_and_run(source);
    source.clear();
    std::cout<<'\n';

    return 0;
}