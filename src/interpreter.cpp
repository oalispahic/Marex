//
// Created by Omar Alispahic on 21. 12. 2025..
//
#include <iostream>
#include "interpreter.hpp"

void Interpreter::run(Program *prog) {
    for (Statement *statements: prog->statements) {
        execStatement(statements);
    }
}

Value Interpreter::evalExpr(Expr *expression) {

    if (auto num = dynamic_cast<NumExpr *>(expression)) {
        return Value::makeInt(num->val);
    }
    if (auto ident = dynamic_cast<IdentExpr *>(expression)) {
        if (!global_scope.count(ident->ident_val))
            throw std::runtime_error("Undefined variable: " + ident->ident_val);
        return global_scope[ident->ident_val];
    }

    if(auto string = dynamic_cast<StringExpr*>(expression)){
        return Value::makeString(string->string_val);
    }

    if (auto bin = dynamic_cast<BinaryExpr *>(expression)) {
        Value left = evalExpr(bin->left);
        Value right = evalExpr(bin->right);

        if (left.type != Type::INT || right.type != Type::INT)
            throw std::runtime_error("Binary operators work only on integer variables! ");

        switch (bin->type) {
            case BinaryOperationType::ADD:
                return Value::makeInt(left.integer_value + right.integer_value);
            case BinaryOperationType::SUB:
                return Value::makeInt(left.integer_value - right.integer_value);
            case BinaryOperationType::MUL:
                return Value::makeInt(left.integer_value * right.integer_value);
            case BinaryOperationType::DIV:
                return Value::makeInt(left.integer_value / right.integer_value);
            case BinaryOperationType::LT:
                return Value::makeInt(left.integer_value < right.integer_value);
            case BinaryOperationType::GT:
                return Value::makeInt(left.integer_value > right.integer_value);
            case BinaryOperationType::EQ:
                return Value::makeInt(left.integer_value == right.integer_value);
            case BinaryOperationType::NOT_EQ:
                return Value::makeInt(left.integer_value != right.integer_value);
            default:
                throw std::runtime_error("Unknown binary operator. ");
        }
    }

}

void Interpreter::execStatement(Statement *statement) {

    if (auto var = dynamic_cast<VarDeclaration_ST *>(statement)) {
        Value val = evalExpr(var->value);
        global_scope[var->var_name] = val;
        return;
    }

    if (auto assign = dynamic_cast<Assign_ST *>(statement)) {
        if (!global_scope.count(assign->var_name))
            throw std::runtime_error("Cannot assign value to undeclared variable " + assign->var_name);
        global_scope[assign->var_name] = evalExpr(assign->value);
        return;
    }

    if (auto print = dynamic_cast<Print_ST *>(statement)) {
        Value val = evalExpr(print->print_value);
        if (val.type == Type::INT)
            std::cout << val.integer_value << '\n';
        else if(val.type == Type::STRING && val.stringValue == "\n")
            std::cout << val.stringValue;
        else if (val.type == Type::STRING)
            std::cout << val.stringValue;
        return;
    }

    if (auto if_statement = dynamic_cast<If_ST *>(statement)) {
        Value condition = evalExpr(if_statement->condition);

        if (condition.integer_value != 0) {
            for (auto body: if_statement->thenBranch) execStatement(body);

        } else {
            for (auto body: if_statement->elseBranch) execStatement(body);
        }
        return;
    }

    if (auto loop = dynamic_cast<Loop_ST *>(statement)) {
        if (loop->initialisation)
            execStatement(loop->initialisation);

        while (evalExpr(loop->condition).integer_value != 0) {
            for (auto s: loop->LoopBody)
                execStatement(s);
            execStatement(loop->increment);
        }
        return;
    }

    if (auto loop = dynamic_cast<RangeLoop_ST *>(statement)) {
        if (!global_scope.count(loop->start) || !global_scope.count(loop->end)) {
            throw std::runtime_error("Start and end vars not declared");
        }
        int start = global_scope[loop->start].integer_value;
        int end = global_scope[loop->end].integer_value;
        for (int i = start; i < end; i++) {
            for (auto body: loop->LoopBody) {
                execStatement(body);
            }
        }
    return;
    }
    throw std::runtime_error("Unknown statement ");
}