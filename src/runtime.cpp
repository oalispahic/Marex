#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include "../include/cli.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include "../include/runtime.hpp"

namespace {
bool read_file(const std::string &file_path, std::string &out) {
    std::ifstream file(file_path);
    if (!file) {
        std::cerr << "marex: cannot open file '" << file_path << "'\n";
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
}
}

int execute_source_once(const std::string &source, const std::vector<std::string> &args) {
    Interpreter interpreter(args);
    int status = EXIT_OK;

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
        status = EXIT_SCRIPT_ERROR;
    }

    // Leave the terminal on a fresh line, but do not add a blank one
    // after programs that already ended their output with newln.
    if (!interpreter.atLineStart()) std::cout << '\n';
    std::cout.flush();
    return status;
}

int execute_script_file(const std::string &file_path, const std::vector<std::string> &args) {
    std::string source;
    if (!read_file(file_path, source)) return EXIT_USAGE_ERROR;
    return execute_source_once(source, args);
}

int dump_tokens_file(const std::string &file_path) {
    std::string source;
    if (!read_file(file_path, source)) return EXIT_USAGE_ERROR;

    Lexer lexer(source);
    const auto tokens = lexer.tokenize();

    std::cout << std::left << std::setw(6) << "line" << std::setw(14) << "type" << "value\n";
    for (const Token &token: tokens) {
        std::string value = token.val;
        if (value == "\n") value = "\\n";
        std::cout << std::left << std::setw(6) << token.token_line
                  << std::setw(14) << tokenTypeName(token.type) << value << '\n';
    }
    return EXIT_OK;
}
