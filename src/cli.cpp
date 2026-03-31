//
// Created by Omar Alispahic on 23. 12. 2025..
//

#include "../include/cli.hpp"

#include <sstream>

CliParseResult parse_cli(int argc, char** argv) {
    CliParseResult result;

    if (argc <= 1) {
        result.options.repl = true;
        return result;
    }

    bool seen_file = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (!seen_file) {
            if (arg == "--") {
                result.error = "Missing script file before --";
                return result;
            }

            if (arg == "-h" || arg == "--help") {
                result.options.show_help = true;
                continue;
            }

            if (arg == "-v" || arg == "--version") {
                result.options.show_version = true;
                continue;
            }

            if (!arg.empty() && arg[0] == '-') {
                result.error = "Unknown option: " + arg;
                return result;
            }

            result.options.file = arg;
            seen_file = true;
            continue;
        }

        result.options.script_args.push_back(arg);
    }

    if (result.options.file.empty() && !result.options.show_help && !result.options.show_version) {
        result.options.repl = true;
    }

    return result;
}

std::string cli_usage() {
    std::ostringstream usage;
    usage << "Usage: marex [options] <file.mx> [args...]\n"
          << "Options:\n"
          << "  -v, --version   Show version\n"
          << "  -h, --help      Show this message\n";
    return usage.str();
}
