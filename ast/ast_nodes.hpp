//
// Created by Omar Alispahic on 1. 11. 2025..
//

#ifndef MAREX_AST_NODES_HPP
#define MAREX_AST_NODES_HPP

#include <string>
#include <vector>

struct Node {
    virtual ~Node() = default;
};

struct Expr : Node {
    virtual ~Expr() = default;
};

struct NumExpr : Expr {
    int val;

    explicit NumExpr(int num_val) : val(num_val) {}
};

struct StringExpr : Expr {
    std::string string_val;

    explicit StringExpr(std::string &string_val) : string_val(string_val) {}
};

struct IdentExpr : Expr {
    std::string ident_val;

    explicit IdentExpr(std::string &ident_val) : ident_val(ident_val) {}
};

enum class BinaryOperationType {
    ADD, SUB, DIV, MUL, LT, GT, EQ, NOT_EQ, ERR
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

struct VarDeclaration_ST : Statement {
    std::string var_name;
    Expr *value;

    VarDeclaration_ST(std::string &var, Expr *value) : var_name(var), value(value) {
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
    std::string start;
    std::string end;
    std::vector<Statement *> LoopBody;

    RangeLoop_ST(std::string &st, std::string &end) : start(st), end(end) {}

    ~RangeLoop_ST() override {
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

struct Program{
    std::vector<Statement*> statements;
    ~Program(){
        for(auto &del: statements) delete del;
    }
};
#endif //MAREX_AST_NODES_HPP
