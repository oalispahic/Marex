//
// Created by Omar Alispahic on 23. 12. 2025..
//

#ifndef MAREX_CLI_HPP
#define MAREX_CLI_HPP

#include <string>
#include <vector>

// Process exit codes. 1 is a problem in the script (syntax or runtime
// error), 2 is a problem with how marex itself was invoked.
enum ExitCode {
    EXIT_OK = 0,
    EXIT_SCRIPT_ERROR = 1,
    EXIT_USAGE_ERROR = 2
};

struct CliOptions {
    bool show_help = false;
    bool show_version = false;
    bool repl = false;
    bool dump_tokens = false;
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
