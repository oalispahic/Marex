//
// Created by Omar Alispahic on 23. 12. 2025..
//

#ifndef MAREX_CLI_HPP
#define MAREX_CLI_HPP

#include <string>
#include <vector>

struct CliOptions {
    bool show_help = false;
    bool show_version = false;
    bool repl = false;
    std::string file;
    std::vector<std::string> script_args;
};

struct CliParseResult {
    CliOptions options;
    std::string error;
};

CliParseResult parse_cli(int argc, char** argv);
std::string cli_usage();

#endif //MAREX_CLI_HPP
