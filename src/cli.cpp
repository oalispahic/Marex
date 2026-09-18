//
// Created by Omar Alispahic on 23. 12. 2025..
//

#include "../include/cli.hpp"

#include <sstream>

CliParseResult parse_cli(int argc, char** argv) {
    CliParseResult result;
    CliOptions &options = result.options;

    bool seen_file = false;
    bool options_ended = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (seen_file) {
            options.script_args.push_back(arg);
            continue;
        }

        if (!options_ended) {
            if (arg == "--") {
                options_ended = true;
                continue;
            }
            if (arg == "-h" || arg == "--help") {
                options.show_help = true;
                continue;
            }
            if (arg == "-v" || arg == "--version") {
                options.show_version = true;
                continue;
            }
            if (arg == "-t" || arg == "--tokens") {
                options.dump_tokens = true;
                continue;
            }
            if (arg.size() > 1 && arg[0] == '-') {
                result.error = "Unknown option: " + arg;
                return result;
            }
        }

        options.file = arg;
        seen_file = true;
    }

    if (options.show_help || options.show_version) return result;

    if (options.file.empty()) {
        if (options.dump_tokens) {
            result.error = "--tokens needs a script file";
            return result;
        }
        options.repl = true;
    }

    return result;
}

std::string cli_usage() {
    std::ostringstream usage;
    usage << "Usage: marex [options] [<file.mx> [args...]]\n"
          << "\n"
          << "Runs a Marex script, or starts the interactive REPL when no file is given.\n"
          << "Arguments after the file are passed to the script as ARGC and ARGV0, ARGV1, ...\n"
          << "\n"
          << "Options:\n"
          << "  -h, --help      Show this message and exit\n"
          << "  -v, --version   Show the version and exit\n"
          << "  -t, --tokens    Print the token stream of the script instead of running it\n"
          << "  --              Treat the next argument as the script file, even if it\n"
          << "                  starts with '-'\n"
          << "\n"
          << "Exit status:\n"
          << "  0  success\n"
          << "  1  syntax or runtime error in the script\n"
          << "  2  invalid command line or unreadable file\n";
    return usage.str();
}
