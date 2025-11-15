//
// Created by Omar Alispahic on 1. 11. 2025..
//

#ifndef MAREX_AST_HPP
#define MAREX_AST_HPP

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

enum class BinaryOperationType {ADD, SUB, DIV, MUL, LT, GT, EQ, NOT_EQ, ERR};

struct BinaryExpr: Expr{
    Expr* left;
    Expr* right;
    BinaryOperationType type;
    BinaryExpr(Expr* left_op, Expr* right_op, BinaryOperationType op) : left(left_op), right(right_op), type(op){}

    ~BinaryExpr() override{
        delete left;
        delete right;
    }
};

// Statements

struct Statement : Node{
    virtual ~Statement() = default;
};





#endif //MAREX_AST_HPP
