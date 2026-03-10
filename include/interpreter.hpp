//
// Created by Omar Alispahic on 21. 12. 2025..
//

#ifndef MAREX_INTERPRETER_HPP
#define MAREX_INTERPRETER_HPP
#include <iostream>
#include <string>
#include <unordered_map>
#include "values_types.hpp"
#include "parser.hpp"



class Interpreter{
    std::unordered_map<std::string,Value> global_scope;

public:
    void run(Program* prog);

private:
    void execStatement(Statement* statement);
    Value evalExpr(Expr* expression);
};


#endif //MAREX_INTERPRETER_HPP
