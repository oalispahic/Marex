//
// Created by Omar Alispahic on 21. 12. 2025..
//
#include <iostream>
#include "../include/interpreter.hpp"

Interpreter::Interpreter(const std::vector<std::string>& args) {
    set_args(args);
}

void Interpreter::set_args(const std::vector<std::string>& args) {
    global_scope["ARGC"] = Value::makeInt(static_cast<int>(args.size()));
    for (size_t i = 0; i < args.size(); ++i) {
        global_scope["ARGV" + std::to_string(i)] = Value::makeString(args[i]);
    }
}

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

        switch (bin->type) {
            case BinaryOperationType::ADD:
                return left.add(right);
            case BinaryOperationType::SUB:
                return left.sub(right);
            case BinaryOperationType::MUL:
                return left.mul(right);
            case BinaryOperationType::DIV:
                return left.div(right);
            case BinaryOperationType::LT:
                return left.lessThan(right);
            case BinaryOperationType::GT:
                return left.greaterThan(right);
            case BinaryOperationType::EQ:
                return left.equals(right);
            case BinaryOperationType::NOT_EQ:
                return left.notEquals(right);
            case BinaryOperationType::AND:
                return left.logicAnd(right);
            case BinaryOperationType::OR:
                return left.logicOr(right);
            default:
                throw std::runtime_error("Unknown binary operator. ");
        }
    }
    return Value::makeNaN();
}

void Interpreter::execStatement(Statement *statement) {

    if (auto var = dynamic_cast<VarDeclaration_ST *>(statement)) {
        if (var->value != nullptr) {
            Value val = evalExpr(var->value);
            global_scope[var->var_name] = val;
            return;
        }
        Value val;
        val.type = Type::NaN;
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
            std::cout << val.integer_value;
        else if (val.type == Type::FLOAT)
            std::cout << val.float_value;
        else if(val.type == Type::STRING && val.stringValue == "\n")
            std::cout << val.stringValue;
        else if (val.type == Type::STRING)
            std::cout << val.stringValue;
        return;
    }

    if (auto system_statement = dynamic_cast<System_ST *>(statement)) {
        std::string statement = system_statement->system_statement;
        system(statement.c_str());
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
