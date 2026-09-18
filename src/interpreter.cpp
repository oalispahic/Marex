//
// Created by Omar Alispahic on 21. 12. 2025..
//
#include <iostream>
#include "../include/interpreter.hpp"

namespace {
// Thrown by 'ret' to unwind to the enclosing call.
struct ReturnSignal {
    Value value;
};

// A runtime error carrying the number of call-trace lines appended so far.
struct TracedError : std::runtime_error {
    int traceLines;
    TracedError(const std::string &message, int traceLines)
        : std::runtime_error(message), traceLines(traceLines) {}
};

const int MAX_TRACE_LINES = 10;

// Restores the frame stack on scope exit even if an exception escapes.
struct FrameGuard {
    std::vector<Scope> &frames;
    explicit FrameGuard(std::vector<Scope> &frames, Scope frame) : frames(frames) {
        frames.push_back(std::move(frame));
    }
    ~FrameGuard() { frames.pop_back(); }
};
}

Interpreter::Interpreter(const std::vector<std::string>& args) {
    frames.emplace_back();
    set_args(args);
}

void Interpreter::set_args(const std::vector<std::string>& args) {
    globals()["ARGC"] = Value::makeInt(static_cast<int>(args.size()));
    for (size_t i = 0; i < args.size(); ++i) {
        globals()["ARGV" + std::to_string(i)] = Value::makeString(args[i]);
    }
}

Scope *Interpreter::scopeOf(const std::string &name) {
    if (currentFrame().count(name)) return &currentFrame();
    if (globals().count(name)) return &globals();
    return nullptr;
}

Value Interpreter::callFunction(CallExpr *call) {
    auto found = functions.find(call->name);
    if (found == functions.end()) {
        throw std::runtime_error("Line " + std::to_string(call->line) + ": Undefined function: " + call->name);
    }
    const std::shared_ptr<Function> function = found->second;   // keep alive during the call

    if (call->arguments.size() != function->parameters.size()) {
        throw std::runtime_error("Line " + std::to_string(call->line) + ": Function " + call->name +
                                 " expects " + std::to_string(function->parameters.size()) +
                                 " argument(s), got " + std::to_string(call->arguments.size()));
    }

    if (static_cast<int>(frames.size()) > MAX_CALL_DEPTH) {
        throw std::runtime_error("Line " + std::to_string(call->line) + ": Stack overflow: call depth exceeds " +
                                 std::to_string(MAX_CALL_DEPTH) + " in " + call->name);
    }

    // Arguments are evaluated in the caller's frame before the new one exists.
    Scope frame;
    for (size_t i = 0; i < call->arguments.size(); ++i) {
        frame[function->parameters[i]] = evalExpr(call->arguments[i]);
    }

    FrameGuard guard(frames, std::move(frame));
    try {
        for (Statement *statement: function->body) execStatement(statement);
        return function->returnValue ? evalExpr(function->returnValue) : Value::makeNaN();
    } catch (const ReturnSignal &early) {
        return early.value;
    } catch (const std::runtime_error &error) {
        // Build a call trace as the error unwinds, innermost call first,
        // and stop adding lines after MAX_TRACE_LINES.
        const auto *traced = dynamic_cast<const TracedError *>(&error);
        const int lines = traced ? traced->traceLines : 0;
        if (lines < MAX_TRACE_LINES) {
            throw TracedError(std::string(error.what()) + "\n  in " + call->name + "()", lines + 1);
        }
        if (lines == MAX_TRACE_LINES) {
            throw TracedError(std::string(error.what()) + "\n  ...", lines + 1);
        }
        throw;
    }
}

void Interpreter::finishLine() {
    if (!at_line_start) {
        std::cout << '\n';
        at_line_start = true;
    }
    std::cout.flush();
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
        Scope *scope = scopeOf(ident->ident_val);
        if (!scope) throw std::runtime_error("Undefined variable: " + ident->ident_val);
        return (*scope)[ident->ident_val];
    }

    if (auto call = dynamic_cast<CallExpr *>(expression)) {
        return callFunction(call);
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

    if (dynamic_cast<NullStmt *>(statement)) return;

    if (auto expr = dynamic_cast<Expr_ST *>(statement)) {
        evalExpr(expr->expression);
        return;
    }

    if (auto var = dynamic_cast<VarDeclaration_ST *>(statement)) {
        Value value = var->value ? evalExpr(var->value) : Value::makeNaN();
        currentFrame()[var->var_name] = value;
        return;
    }

    if (auto assign = dynamic_cast<Assign_ST *>(statement)) {
        Value value = evalExpr(assign->value);
        Scope *scope = scopeOf(assign->var_name);
        if (!scope) throw std::runtime_error("Cannot assign value to undeclared variable " + assign->var_name);
        (*scope)[assign->var_name] = value;
        return;
    }

    if (auto declaration = dynamic_cast<FunDecl_ST *>(statement)) {
        functions[declaration->function->name] = declaration->function;
        return;
    }

    if (auto ret = dynamic_cast<Return_ST *>(statement)) {
        throw ReturnSignal{ret->value ? evalExpr(ret->value) : Value::makeNaN()};
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
