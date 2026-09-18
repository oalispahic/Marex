#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include "../include/runtime.hpp"

bool execute_source_once(const std::string &source, const std::vector<std::string> &args) {
    Interpreter interpreter(args);
    bool ok = true;

    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<Program> program(parser.parse());

        interpreter.run(program.get());
    } catch (const std::exception &e) {
        if (!interpreter.atLineStart()) std::cout << '\n';
        std::cout.flush();
        std::cerr << e.what() << '\n';
        ok = false;
    }

    // Leave the terminal on a fresh line, but do not add a blank one
    // after programs that already ended their output with newln.
    if (!interpreter.atLineStart()) std::cout << '\n';
    std::cout.flush();
    return ok;
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
