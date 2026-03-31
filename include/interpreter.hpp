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

public:
    explicit Interpreter(const std::vector<std::string>& args = {});
    void run(Program* prog);
    void set_args(const std::vector<std::string>& args);

private:
    void execStatement(Statement* statement);
    Value evalExpr(Expr* expression);
};


#endif //MAREX_INTERPRETER_HPP
