#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include "../include/repl.hpp"
#include "version.hpp"

namespace {

const char *const PRIMARY_PROMPT = ">>> ";
const char *const CONTINUATION_PROMPT = "... ";

bool stdin_is_terminal() {
    return isatty(STDIN_FILENO) != 0;
}

void clear_terminal() {
    if (!stdin_is_terminal()) return;
    std::cout << "\033[2J\033[H" << std::flush;
}

// Puts the terminal into non-canonical mode for the lifetime of the
// object so single key presses (arrows, Ctrl-C, Ctrl-D) can be read.
class RawTerminalMode {
public:
    RawTerminalMode() : enabled(false) {
        if (!stdin_is_terminal()) return;
        if (tcgetattr(STDIN_FILENO, &original) == -1) return;

        termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO | ISIG);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) enabled = true;
    }

    ~RawTerminalMode() {
        if (enabled) tcsetattr(STDIN_FILENO, TCSANOW, &original);
    }

    bool isEnabled() const { return enabled; }

private:
    termios original{};
    bool enabled;
};

struct ReadResult {
    enum Kind { LINE, END_OF_INPUT, INTERRUPT } kind;
    std::string line;
};

// Reads one line with a plain getline; used when stdin is not a terminal.
ReadResult read_plain_line(const std::string &prompt) {
    std::string line;
    std::cout << prompt << std::flush;
    if (!std::getline(std::cin, line)) return {ReadResult::END_OF_INPUT, ""};
    return {ReadResult::LINE, line};
}

// Skips the rest of an escape sequence such as ESC [ 3 ~ after ESC was read.
// Returns the final byte, or 0 if the sequence could not be read.
char read_escape_sequence(char &introducer) {
    if (read(STDIN_FILENO, &introducer, 1) <= 0) return 0;
    if (introducer != '[' && introducer != 'O') return introducer;

    char c = 0;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        if (c >= 0x40 && c <= 0x7E) return c;   // final byte of a CSI sequence
    }
    return 0;
}

ReadResult read_repl_line(const std::string &prompt, const std::vector<std::string> &history) {
    if (!stdin_is_terminal()) return read_plain_line(prompt);

    RawTerminalMode rawMode;
    if (!rawMode.isEnabled()) return read_plain_line(prompt);

    std::string line;
    size_t historyIndex = history.size();
    std::cout << prompt << std::flush;

    auto redraw = [&]() { std::cout << "\r\033[K" << prompt << line << std::flush; };

    while (true) {
        char c = 0;
        const ssize_t got = read(STDIN_FILENO, &c, 1);
        if (got == 0) {
            std::cout << '\n';
            return {ReadResult::END_OF_INPUT, ""};
        }
        if (got < 0) continue;

        if (c == '\n' || c == '\r') {
            std::cout << '\n';
            return {ReadResult::LINE, line};
        }

        if (c == 3) {                               // Ctrl-C
            std::cout << "^C\n";
            return {ReadResult::INTERRUPT, ""};
        }

        if (c == 4) {                               // Ctrl-D
            if (line.empty()) {
                std::cout << '\n';
                return {ReadResult::END_OF_INPUT, ""};
            }
            continue;
        }

        if (c == 12) {                              // Ctrl-L
            clear_terminal();
            redraw();
            continue;
        }

        if (c == 21) {                              // Ctrl-U
            line.clear();
            redraw();
            continue;
        }

        if (c == 127 || c == '\b') {
            if (!line.empty()) {
                line.pop_back();
                redraw();
            }
            continue;
        }

        if (c == 27) {
            char introducer = 0;
            const char final = read_escape_sequence(introducer);
            if (introducer != '[' && introducer != 'O') continue;

            if (final == 'A' && historyIndex > 0) {              // up
                --historyIndex;
                line = history[historyIndex];
                redraw();
            } else if (final == 'B' && historyIndex < history.size()) {   // down
                ++historyIndex;
                line = historyIndex < history.size() ? history[historyIndex] : "";
                redraw();
            }
            continue;
        }

        if (isprint(static_cast<unsigned char>(c))) {
            line.push_back(c);
            std::cout << c << std::flush;
        }
    }
}

void print_help() {
    std::cout << "Commands:\n"
              << "  :help    Show this message\n"
              << "  :info    List defined functions and variables (type, value, memory use)\n"
              << "  :reset   Forget all variables\n"
              << "  :clear   Clear the screen\n"
              << "  :exit    Leave the REPL (also Ctrl-D)\n"
              << "Ctrl-C cancels the statement being typed.\n"
              << "Multi-line statements (if/loop) run once the closing fi/done is entered.\n";
}

void print_banner() {
    clear_terminal();
    std::cout << "Marex " << MAREX_VERSION_STRING << " REPL\n"
              << "Type :help for commands, :exit to leave.\n";
}

// Approximate heap + inline footprint of one variable entry.
size_t approx_bytes(const std::string &name, const Value &value) {
    size_t bytes = sizeof(std::pair<const std::string, Value>);
    if (name.capacity() > sizeof(std::string)) bytes += name.capacity();
    if (value.stringValue.capacity() > sizeof(std::string)) bytes += value.stringValue.capacity();
    return bytes;
}

std::string display_value(const Value &value) {
    if (value.type != Type::STRING) return value.toString();
    std::string text;
    for (char c: value.stringValue) {
        if (c == '\n') text += "\\n";
        else text += c;
    }
    return "\"" + text + "\"";
}

void print_functions(const Interpreter &interpreter) {
    const auto &functions = interpreter.definedFunctions();
    if (functions.empty()) return;

    std::vector<std::string> names;
    for (const auto &entry: functions) names.push_back(entry.first);
    std::sort(names.begin(), names.end());

    std::cout << "Functions:\n";
    for (const std::string &name: names) {
        const Function &function = *functions.at(name);
        std::cout << "  " << name << "(";
        for (size_t i = 0; i < function.parameters.size(); ++i) {
            if (i) std::cout << ", ";
            std::cout << function.parameters[i];
        }
        std::cout << ")" << (function.returnValue ? "" : "  [void]") << '\n';
    }
}

void print_info(const Interpreter &interpreter) {
    print_functions(interpreter);

    const auto &variables = interpreter.variables();
    if (variables.empty()) {
        std::cout << "No variables defined.\n";
        return;
    }

    std::vector<std::string> names;
    names.reserve(variables.size());
    size_t nameWidth = 4;
    for (const auto &entry: variables) {
        names.push_back(entry.first);
        nameWidth = std::max(nameWidth, entry.first.size());
    }
    std::sort(names.begin(), names.end());

    const int valueWidth = 24;
    std::cout << std::left << std::setw(static_cast<int>(nameWidth) + 2) << "name"
              << std::setw(8) << "type" << std::setw(valueWidth) << "value" << "bytes\n";

    size_t totalBytes = 0;
    for (const std::string &name: names) {
        const Value &value = variables.at(name);
        std::string shown = display_value(value);
        if (shown.size() > static_cast<size_t>(valueWidth - 2)) shown = shown.substr(0, valueWidth - 5) + "...";

        const size_t bytes = approx_bytes(name, value);
        totalBytes += bytes;
        std::cout << std::left << std::setw(static_cast<int>(nameWidth) + 2) << name
                  << std::setw(8) << typeName(value.type) << std::setw(valueWidth) << shown
                  << bytes << '\n';
    }
    std::cout << variables.size() << " variable" << (variables.size() == 1 ? "" : "s")
              << ", about " << totalBytes << " bytes\n";
}

// Handles a ':' command. Returns false if the REPL should stop.
bool run_command(const std::string &command, std::unique_ptr<Interpreter> &interpreter) {
    if (command == ":exit" || command == ":quit" || command == ":q") return false;

    if (command == ":help" || command == ":h") print_help();
    else if (command == ":info") print_info(*interpreter);
    else if (command == ":reset") {
        interpreter.reset(new Interpreter({}));
        std::cout << "All variables forgotten.\n";
    } else if (command == ":clear") print_banner();
    else std::cout << "Unknown command " << command << ". Type :help for a list.\n";
    return true;
}

void run_buffered(Interpreter &interpreter, const std::vector<Token> &tokens) {
    try {
        Parser parser(tokens);
        std::unique_ptr<Program> program(parser.parse());
        interpreter.run(program.get());
    } catch (const std::exception &e) {
        interpreter.finishLine();
        std::cerr << e.what() << '\n';
    }
    interpreter.finishLine();
}

} // namespace

void repl() {
    std::string bufferedProgram;
    std::vector<std::string> history;
    std::unique_ptr<Interpreter> interpreter(new Interpreter({}));

    print_banner();

    while (true) {
        const std::string prompt = bufferedProgram.empty() ? PRIMARY_PROMPT : CONTINUATION_PROMPT;
        const ReadResult input = read_repl_line(prompt, history);

        if (input.kind == ReadResult::END_OF_INPUT) break;
        if (input.kind == ReadResult::INTERRUPT) {
            bufferedProgram.clear();
            continue;
        }

        const std::string &line = input.line;
        if (!line.empty() && (history.empty() || history.back() != line)) history.push_back(line);

        // ':' commands work even while a multi-line statement is pending;
        // ':=' cannot start a statement so there is no ambiguity.
        if (line.size() > 1 && line[0] == ':' && line[1] != '=') {
            if (!run_command(line, interpreter)) break;
            continue;
        }

        bufferedProgram += line + "\n";

        Lexer lexer(bufferedProgram);
        auto tokens = lexer.tokenize();
        if (lexer.inOpenBlockComment()) continue;

        const ParseStatus status = Parser::getParseStatus(tokens);
        if (status == ParseStatus::WAIT) continue;

        // COMPLETE runs the program; ERR runs it too so the parser's own
        // message is what the user sees.
        run_buffered(*interpreter, tokens);
        bufferedProgram.clear();
    }

    if (stdin_is_terminal()) std::cout << "Bye.\n";
}
