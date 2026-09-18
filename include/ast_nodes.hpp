//
// Created by Omar Alispahic on 1. 11. 2025..
//

#ifndef MAREX_AST_NODES_HPP
#define MAREX_AST_NODES_HPP

#include <memory>
#include <string>
#include <vector>

struct Node {
    virtual ~Node() = default;
};

struct Expr : Node {
    virtual ~Expr() = default;
};

struct NullExpr : Expr {
    virtual ~NullExpr() = default;
};

struct NumExpr : Expr {
    int val;
    float floatValue;
    bool isFloat;

    explicit NumExpr(int num_val) : val(num_val), floatValue(0), isFloat(false) {}

    static NumExpr *makeFloat(float float_val) {
        NumExpr *expr = new NumExpr(0);
        expr->floatValue = float_val;
        expr->isFloat = true;
        return expr;
    }
};

struct StringExpr : Expr {
    std::string string_val;

    explicit StringExpr(const std::string &string_val) : string_val(string_val) {}
};

struct IdentExpr : Expr {
    std::string ident_val;

     IdentExpr(const std::string &ident_val) : ident_val(ident_val) {}
};

struct ArrayExpr : Expr {
    std::vector<Expr *> elements;
};

struct ArrayIndexExpr : Expr {
    Expr *array;
    Expr *index;
};

// A call such as 'name(arg, arg)'.
struct CallExpr : Expr {
    std::string name;
    std::vector<Expr *> arguments;
    int line;

    CallExpr(const std::string &name, int line) : name(name), line(line) {}

    ~CallExpr() override {
        for (auto &arg: arguments) delete arg;
    }
};

enum class BinaryOperationType {
    ADD, SUB, DIV, MUL, LT, GT, EQ, NOT_EQ, OR, AND
};

struct BinaryExpr : Expr {
    Expr *left;
    Expr *right;
    BinaryOperationType type;

    BinaryExpr(Expr *left_op, Expr *right_op, BinaryOperationType op) : left(left_op), right(right_op), type(op) {}

    ~BinaryExpr() override {
        delete left;
        delete right;
    }
};

// Statements

struct Statement : Node {
    virtual ~Statement() = default;
};

struct NullStmt : Statement {
    virtual ~NullStmt() = default;
};

// A bare expression used as a statement. It is evaluated for its side
// effects (and errors) and the result is discarded.
struct Expr_ST : Statement {
    Expr *expression;

    explicit Expr_ST(Expr *expression) : expression(expression) {}

    ~Expr_ST() override { delete expression; }
};
struct VarDeclaration_ST : Statement {
    std::string var_name;
    bool isArray;
    Expr *value;

    VarDeclaration_ST(const std::string &var, Expr *value) : var_name(var), value(value) {
    }

    ~VarDeclaration_ST() override {
        delete value;
    }
};

struct Assign_ST : Statement {
    std::string var_name;
    Expr *value;

    Assign_ST(std::string var_name, Expr *value) : var_name(var_name), value(value) {}

    ~Assign_ST() override { delete value; }
};

struct Print_ST : Statement {
    Expr *print_value;

    explicit Print_ST(Expr *print_value) : print_value(print_value) {}

    ~Print_ST() override { delete print_value; }

};

struct System_ST : Statement {
    std::string system_statement;

    explicit System_ST(const std::string &system_statement) : system_statement(system_statement) {}
};

struct Loop_ST : Statement {
    Statement *initialisation;
    Expr *condition;
    Statement *increment;
    std::vector<Statement *> LoopBody;

    Loop_ST(Statement *init, Expr *condition, Statement *increment) : initialisation(init), condition(condition),
                                                                      increment(increment) {}

    ~Loop_ST() override {
        if (initialisation) delete initialisation;
        delete condition;
        delete increment;
        for (auto &del: LoopBody) delete del;
    }
};

struct RangeLoop_ST : Statement {
    Expr *start;
    Expr *end;
    Expr *step;
    std::vector<Statement *> LoopBody;

    RangeLoop_ST(Expr *st, Expr *en, Expr *stp) : start(st), end(en), step(stp) {}

    ~RangeLoop_ST() override {
        delete start;
        delete end;
        if (step) delete step;
        for (auto &del: LoopBody) delete del;
    }
};

struct If_ST : Statement{
    Expr* condition;
    std::vector<Statement*> thenBranch;
    std::vector<Statement*> elseBranch;

    If_ST(Expr* cond) : condition(cond){}

    ~If_ST(){
        delete condition;
        for(auto &del: thenBranch) delete del;
        for(auto &del : elseBranch) delete del;
    }
};

// Early 'ret' inside an if/loop within a function body.
struct Return_ST : Statement {
    Expr *value;   // nullptr for a void return

    explicit Return_ST(Expr *value) : value(value) {}

    ~Return_ST() override { delete value; }
};

// A function definition. It is shared between the AST and the
// interpreter's function table so that a function defined in one REPL
// input survives the deletion of that input's Program.
struct Function {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<Statement *> body;
    Expr *returnValue = nullptr;   // the closing 'ret'; nullptr for void

    ~Function() {
        for (auto &del: body) delete del;
        delete returnValue;
    }
};

struct FunDecl_ST : Statement {
    std::shared_ptr<Function> function;

    explicit FunDecl_ST(std::shared_ptr<Function> function) : function(std::move(function)) {}
};

struct Program{
    std::vector<Statement*> statements;
    ~Program(){
        for(auto &del: statements) delete del;
    }
};
#endif //MAREX_AST_NODES_HPP
