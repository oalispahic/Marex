//
// Created by Omar Alispahic on 21. 12. 2025..
//

#ifndef MAREX_INTERPRETER_HPP
#define MAREX_INTERPRETER_HPP
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "values_types.hpp"
#include "parser.hpp"



class Interpreter{
    std::unordered_map<std::string,Value> global_scope;
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

    // Every variable currently defined, including ARGC/ARGV*.
    const std::unordered_map<std::string, Value> &variables() const { return global_scope; }

private:
    void execStatement(Statement* statement);
    Value evalExpr(Expr* expression);
};


#endif //MAREX_INTERPRETER_HPP
