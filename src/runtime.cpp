#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include "../include/runtime.hpp"

bool execute_source_once(const std::string &source, const std::vector<std::string> &args) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<Program> program(parser.parse());

        Interpreter interpreter(args);
        interpreter.run(program.get());
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

bool execute_script_file(const std::string &file_path, const std::vector<std::string> &args) {
    std::ifstream file(file_path);
    if (!file) {
        std::cerr << "Error opening file " << file_path << '\n';
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string source = buffer.str();
    return execute_source_once(source, args);
}
