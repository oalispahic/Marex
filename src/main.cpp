//
// Created by Omar Alispahic on 23. 12. 2025..
//

#include <iostream>
#include "../include/cli.hpp"
#include "../include/repl.hpp"
#include "../include/runtime.hpp"
#include "../include/version.hpp.in"

namespace {
int handle_cli_errors(const CliParseResult &parsed) {
    if (parsed.error.empty()) return 0;
    std::cerr << parsed.error << '\n';
    std::cerr << cli_usage();
    return 1;
}

bool handle_meta_options(const CliParseResult &parsed) {
    if (parsed.options.show_version) {
        std::cout << "Marex " << MAREX_VERSION_STRING << '\n';
        return true;
    }

    if (parsed.options.show_help) {
        std::cout << cli_usage();
        return true;
    }

    if (parsed.options.repl) {
        repl();
        return true;
    }

    return false;
}
} // namespace

int main(int argc, char** argv){
    CliParseResult parsed = parse_cli(argc, argv);
    const int cliError = handle_cli_errors(parsed);
    if (cliError != 0) return cliError;

    if (handle_meta_options(parsed)) return 0;

    execute_script_file(parsed.options.file, parsed.options.script_args);
    std::cout<<'\n';
    return 0;
}
