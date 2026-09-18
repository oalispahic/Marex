#ifndef MAREX_STYLE_HPP
#define MAREX_STYLE_HPP

#include <string>

// Light terminal styling. Codes are emitted only when the stream is a
// terminal; piped output stays plain. NO_COLOR disables colours,
// CLICOLOR_FORCE / FORCE_COLOR enables them even when piped.
namespace style {

// A styler bound to stdout or stderr.
class Styler {
public:
    explicit Styler(bool enabled) : enabled(enabled) {}

    bool isEnabled() const { return enabled; }

    std::string bold(const std::string &text) const { return wrap("1", text); }
    std::string dim(const std::string &text) const { return wrap("2", text); }
    std::string red(const std::string &text) const { return wrap("31", text); }
    std::string green(const std::string &text) const { return wrap("32", text); }
    std::string yellow(const std::string &text) const { return wrap("33", text); }
    std::string cyan(const std::string &text) const { return wrap("36", text); }

    // Marex orange (#E37719), falling back to yellow on 16-colour terminals.
    std::string accent(const std::string &text) const;
    std::string accentBold(const std::string &text) const;

private:
    bool enabled;
    std::string wrap(const char *code, const std::string &text) const;
};

const Styler &out();   // for stdout
const Styler &err();   // for stderr

// Prints a runtime or syntax error to stderr: the first line in red,
// call-trace lines ("  in f()", "  ...") dimmed.
void printError(const std::string &message);

}

#endif //MAREX_STYLE_HPP
