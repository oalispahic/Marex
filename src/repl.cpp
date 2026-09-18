#include <cctype>
#include <iostream>
#include <memory>
#include <termios.h>
#include <unistd.h>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include "../include/repl.hpp"
#include "../include/version.hpp.in"

namespace {
void clear_terminal() {
#if defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
    system("cls");
#else
    system("clear");
#endif
}

class RawTerminalMode {
public:
    RawTerminalMode() : enabled(false) {
        if (!isatty(STDIN_FILENO)) return;
        if (tcgetattr(STDIN_FILENO, &original) == -1) return;

        termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0) enabled = true;
    }

    ~RawTerminalMode() {
        if (enabled) tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    }

    bool isEnabled() const { return enabled; }

private:
    termios original{};
    bool enabled;
};

std::string read_repl_line(const std::string &prompt, const std::vector<std::string> &history) {
    if (!isatty(STDIN_FILENO)) {
        std::string line;
        std::cout << prompt;
        std::getline(std::cin, line);
        return line;
    }

    RawTerminalMode rawMode;
    if (!rawMode.isEnabled()) {
        std::string line;
        std::cout << prompt;
        std::getline(std::cin, line);
        return line;
    }

    std::string line;
    int historyIndex = static_cast<int>(history.size());
    std::cout << prompt << std::flush;

    auto redraw = [&]() { std::cout << "\r\033[K" << prompt << line << std::flush; };

    while (true) {
        char c = 0;
        if (read(STDIN_FILENO, &c, 1) <= 0) continue;

        if (c == '\n' || c == '\r') {
            std::cout << '\n';
            break;
        }

        if ((c == 127 || c == '\b') && !line.empty()) {
            line.pop_back();
            redraw();
            continue;
        }

        if (c == 27) {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) continue;
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) continue;

            if (seq[0] == '[' && seq[1] == 'A') {
                if (!history.empty() && historyIndex > 0) {
                    --historyIndex;
                    line = history[historyIndex];
                    redraw();
                }
            } else if (seq[0] == '[' && seq[1] == 'B') {
                if (historyIndex < static_cast<int>(history.size()) - 1) {
                    ++historyIndex;
                    line = history[historyIndex];
                    redraw();
                } else if (historyIndex == static_cast<int>(history.size()) - 1) {
                    ++historyIndex;
                    line.clear();
                    redraw();
                }
            }
            continue;
        }

        if (isprint(static_cast<unsigned char>(c))) {
            line.push_back(c);
            std::cout << c << std::flush;
        }
    }

    return line;
}

void print_repl_banner() {
    clear_terminal();
    std::cout << "Marex " << MAREX_VERSION_STRING << '\n';
    std::cout << "Commands: :exit, :clear\n";
}

}

void repl() {
    std::string line;
    std::string bufferedProgram;
    std::vector<std::string> history;
    Interpreter interpreter({});

    print_repl_banner();

    while (true) {
        const std::string prompt = bufferedProgram.empty() ? ">>> " : ".....> ";
        line = read_repl_line(prompt, history);

        if (line == ":clear") {
            print_repl_banner();
            bufferedProgram.clear();
            continue;
        }

        if (line == ":exit") {
            std::cout << '\n' << "Exiting..." << '\n';
            break;
        }

        if (!line.empty()) history.push_back(line);
        bufferedProgram += line + "\n";

        Lexer lexer(bufferedProgram);
        auto tokens = lexer.tokenize();
        const ParseStatus status = Parser::getParseStatus(tokens);

        if (status == ParseStatus::WAIT) continue;

        if (status == ParseStatus::ERR) {
            try {
                Parser parser(tokens);
                std::unique_ptr<Program> invalid(parser.parse());
            } catch (const std::exception &e) {
                std::cerr << e.what() << '\n';
            }
            bufferedProgram.clear();
            continue;
        }

        try {
            Parser parser(tokens);
            std::unique_ptr<Program> parsedProgram(parser.parse());
            interpreter.run(parsedProgram.get());
        } catch (const std::exception &e) {
            if (!interpreter.atLineStart()) std::cout << '\n';
            std::cout.flush();
            std::cerr << e.what() << '\n';
        }

        if (!interpreter.atLineStart()) std::cout << '\n';
        std::cout.flush();
        bufferedProgram.clear();
    }
}
