#ifndef MAREX_RUNTIME_HPP
#define MAREX_RUNTIME_HPP

#include <string>
#include <vector>

// Lexes, parses and runs one complete program. Errors are printed to
// stderr; the return value is an ExitCode from cli.hpp.
int execute_source_once(const std::string &source, const std::vector<std::string> &args);

// Reads the file and runs it with execute_source_once.
int execute_script_file(const std::string &file_path, const std::vector<std::string> &args);

// Prints the token stream of a file, one token per line.
int dump_tokens_file(const std::string &file_path);

#endif //MAREX_RUNTIME_HPP
