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
        if (num->isFloat) return Value::makeFloat(num->floatValue);
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
        global_scope[var->var_name] = var->value ? evalExpr(var->value) : Value::makeNaN();
        return;
    }

    if (auto assign = dynamic_cast<Assign_ST *>(statement)) {
        if (!global_scope.count(assign->var_name))
            throw std::runtime_error("Cannot assign value to undeclared variable " + assign->var_name);
        global_scope[assign->var_name] = evalExpr(assign->value);
        return;
    }

    if (auto print = dynamic_cast<Print_ST *>(statement)) {
        const std::string text = evalExpr(print->print_value).toString();
        if (text.empty()) return;
        std::cout << text;
        at_line_start = text.back() == '\n';
        return;
    }

    if (auto system_statement = dynamic_cast<System_ST *>(statement)) {
        std::cout.flush();
        std::string statement = system_statement->system_statement;
        system(statement.c_str());
        at_line_start = true;
        return;
    }


    if (auto if_statement = dynamic_cast<If_ST *>(statement)) {
        if (evalExpr(if_statement->condition).truthy()) {
            for (auto body: if_statement->thenBranch) execStatement(body);

        } else {
            for (auto body: if_statement->elseBranch) execStatement(body);
        }
        return;
    }

    if (auto loop = dynamic_cast<Loop_ST *>(statement)) {
        if (loop->initialisation)
            execStatement(loop->initialisation);

        while (evalExpr(loop->condition).truthy()) {
            for (auto s: loop->LoopBody)
                execStatement(s);
            execStatement(loop->increment);
        }
        return;
    }

    if (auto loop = dynamic_cast<RangeLoop_ST *>(statement)) {
        Value startValue = evalExpr(loop->start);
        Value endValue = evalExpr(loop->end);

        if (startValue.type != Type::INT || endValue.type != Type::INT) {
            throw std::runtime_error("Range loop supports integer start/end values only. ");
        }

        int start = startValue.integer_value;
        int end = endValue.integer_value;
        int step = 0;

        if (loop->step) {
            Value stepValue = evalExpr(loop->step);
            if (stepValue.type != Type::INT) {
                throw std::runtime_error("Range loop step must be an integer value. ");
            }
            step = stepValue.integer_value;
        } else {
            step = (start < end) ? 1 : -1;
        }

        if (step == 0) {
            throw std::runtime_error("Range loop step cannot be 0. ");
        }

        if ((start < end && step < 0) || (start > end && step > 0)) {
            throw std::runtime_error("Range loop step direction does not match start/end values. ");
        }

        if (start < end) {
            for (int i = start; i < end; i += step) {
                for (auto body: loop->LoopBody) {
                    execStatement(body);
                }
            }
        } else if (start > end) {
            for (int i = start; i > end; i += step) {
                for (auto body: loop->LoopBody) {
                    execStatement(body);
                }
            }
        }

        return;
    }
    throw std::runtime_error("Unknown statement ");
}
