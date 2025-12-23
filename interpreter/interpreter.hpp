//
// Created by Omar Alispahic on 21. 12. 2025..
//

#ifndef MAREX_INTERPRETER_HPP
#define MAREX_INTERPRETER_HPP
#include <iostream>
#include <string>
#include <unordered_map>
#include "../parser/parser.hpp"

enum class Type{
    INT,
    STRING,
    BOOL,
    NaN
};

struct Value{
    Type type = Type::NaN;
    int integer_value = 0;
    std::string stringValue{};
    bool boolVal;

    static Value makeInt(int v){
        Value val;
        val.type = Type::INT;
        val.integer_value = v;
        return val;
    }

    static Value makeString(const std::string &s){
        Value val;
        val.type = Type::STRING;
        val.stringValue = s;
        return val;
    }

    static Value makeBool(int b){
        Value val;
        val.type = Type::BOOL;
        val.boolVal = (b>=1);
    }
};

class Interpreter{
    std::unordered_map<std::string,Value> global_scope;

public:
    void run(Program* prog);

private:
    void execStatement(Statement* statement);
    Value evalExpr(Expr* expression);
};


#endif //MAREX_INTERPRETER_HPP
