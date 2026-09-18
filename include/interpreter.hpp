//
// Created by Omar Alispahic on 21. 12. 2025..
//

#ifndef MAREX_INTERPRETER_HPP
#define MAREX_INTERPRETER_HPP
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "values_types.hpp"
#include "parser.hpp"

using Scope = std::unordered_map<std::string, Value>;

class Interpreter{
    // frames[0] holds the globals; every function call pushes a frame
    // with its parameters and local 'var' declarations and pops it on
    // return. Name lookup checks the current frame, then the globals.
    std::vector<Scope> frames;
    std::unordered_map<std::string, std::shared_ptr<Function>> functions;
    bool at_line_start = true;

public:
    explicit Interpreter(const std::vector<std::string>& args = {});
    void run(Program* prog);
    void set_args(const std::vector<std::string>& args);

    // True when the last thing written to stdout ended with a newline
    // (or nothing was written yet). Lets callers finish a dangling line
    // without adding blank lines after programs that end with newln.
    bool atLineStart() const { return at_line_start; }

    // Ends a dangling output line with a newline, if there is one, and
    // flushes stdout.
    void finishLine();

    // Every global variable currently defined, including ARGC/ARGV*.
    const Scope &variables() const { return frames.front(); }

    // Every function currently defined.
    const std::unordered_map<std::string, std::shared_ptr<Function>> &definedFunctions() const { return functions; }

    // Deepest allowed nesting of function calls.
    static const int MAX_CALL_DEPTH = 1000;

private:
    void execStatement(Statement* statement);
    Value evalExpr(Expr* expression);
    Value callFunction(CallExpr *call);

    Scope &currentFrame() { return frames.back(); }
    Scope &globals() { return frames.front(); }

    // The scope a name resolves to, or nullptr if it is not defined.
    Scope *scopeOf(const std::string &name);
};


#endif //MAREX_INTERPRETER_HPP
