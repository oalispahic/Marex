//
// Created by Omar Alispahic on 21. 12. 2025..
//

#include <iostream>
#include "../include/cli.hpp"
#include "../include/repl.hpp"
#include "../include/runtime.hpp"
#include "version.hpp"

int main(int argc, char** argv){
    const CliParseResult parsed = parse_cli(argc, argv);
    const CliOptions &options = parsed.options;

    if (!parsed.error.empty()) {
        std::cerr << "marex: " << parsed.error << "\n\n" << cli_usage();
        return EXIT_USAGE_ERROR;
    }

    if (options.show_help) {
        std::cout << cli_usage();
        return EXIT_OK;
    }

    if (options.show_version) {
        std::cout << "Marex " << MAREX_VERSION_STRING << '\n';
        return EXIT_OK;
    }

    if (options.repl) {
        repl();
        return EXIT_OK;
    }

    if (options.dump_tokens) {
        return dump_tokens_file(options.file);
    }

    return execute_script_file(options.file, options.script_args);
}
