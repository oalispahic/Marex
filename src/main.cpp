//
// Created by Omar Alispahic on 23. 12. 2025..
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include "../include/cli.hpp"
#include "../include/version.hpp.in"


void clear_terminal(){
#if defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
    system("cls");
#else
    system("clear");
#endif
}
void compile_and_run(const std::string &source, const std::vector<std::string>& args){
    try{
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto program = parser.parse();

        Interpreter interpreter(args);
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
    std::cout<<"Welcome to Marex REPL (type :run to execute, :exit to quit, :clear to clear terminal)\n";

    while(true){
        std::cout<<"marex> ";
        std::getline(std::cin,line);

        if (line ==":clear") {
            clear_terminal();
            line.clear();
        }
        if(line==":exit"){
            std::cout<<'\n'<<"Quit!"<<'\n';
            break;
        }
        if(line == ":run"){
            compile_and_run(program, {});
            std::cout<<'\n';
            program.clear();
            continue;
        }

        program += line + "\n";
    }
}


int main(int argc, char** argv){

    CliParseResult parsed = parse_cli(argc, argv);
    if (!parsed.error.empty()) {
        std::cerr << parsed.error << '\n';
        std::cerr << cli_usage();
        return 1;
    }

    if (parsed.options.show_version) {
        std::cout << "Marex " << MAREX_VERSION_STRING << '\n';
        return 0;
    }

    if (parsed.options.show_help) {
        std::cout << cli_usage();
        return 0;
    }

    if (parsed.options.repl) {
        repl();
        return 0;
    }

    std::ifstream file(parsed.options.file);
    if(!file){
        std::cerr<<"Error opening file "<<parsed.options.file<<'\n';
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    compile_and_run(source, parsed.options.script_args);
    source.clear();
    std::cout<<'\n';

    return 0;
}
