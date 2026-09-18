#include "../include/style.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace style {

namespace {
bool env_set(const char *name) {
    const char *value = std::getenv(name);
    return value != nullptr && *value != '\0';
}

bool colours_wanted(int fd) {
    if (env_set("NO_COLOR")) return false;
    if (env_set("CLICOLOR_FORCE") || env_set("FORCE_COLOR")) return true;
    const char *term = std::getenv("TERM");
    if (term && std::strcmp(term, "dumb") == 0) return false;
    return isatty(fd) != 0;
}

bool supports_256_colours() {
    if (env_set("COLORTERM")) return true;
    const char *term = std::getenv("TERM");
    return term && std::strstr(term, "256color") != nullptr;
}
}

std::string Styler::wrap(const char *code, const std::string &text) const {
    if (!enabled || text.empty()) return text;
    return std::string("\033[") + code + "m" + text + "\033[0m";
}

std::string Styler::accent(const std::string &text) const {
    return wrap(supports_256_colours() ? "38;5;208" : "33", text);
}

std::string Styler::accentBold(const std::string &text) const {
    return wrap(supports_256_colours() ? "1;38;5;208" : "1;33", text);
}

const Styler &out() {
    static const Styler styler(colours_wanted(STDOUT_FILENO));
    return styler;
}

const Styler &err() {
    static const Styler styler(colours_wanted(STDERR_FILENO));
    return styler;
}

void printError(const std::string &message) {
    std::istringstream lines(message);
    std::string line;
    bool first = true;
    while (std::getline(lines, line)) {
        if (first) {
            std::cerr << err().red(line) << '\n';
            first = false;
        } else {
            std::cerr << err().dim(line) << '\n';
        }
    }
}

}
