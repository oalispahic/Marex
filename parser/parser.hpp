//
// Created by Omar Alispahic on 15. 11. 2025..
//

#ifndef MAREX_PARSER_HPP
#define MAREX_PARSER_HPP

#include "token_.hpp"
#include "ast/ast_nodes.hpp"
#include <vector>
#include <string>

class Parser {
    std::vector<Token> tokens;
    size_t current_token = 0;

public:
    explicit Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    Program *parse();

private:

    //---------PARSE STMT----------

    Statement *parseStatement();

    Statement *parseVarDeclaration();

    Statement *parseAssign();

    Statement *parsePrint();

    Statement *parseIf();

    Statement *parseLoop();

    //---------PARSE EXPR----------

    Expr *parseExpr();

    Expr *parseLogicOr();

    Expr *parseLogicAnd();

    Expr *parseLogicEqual();

    Expr *parseCompare();

    Expr *parseTerm();

    Expr *parseFactor();

    Expr *parseUnary();

    Expr *parsePrimary();

    //---------HELPERS----------

    const Token &peek() const;

    const Token &previous() const;

    bool isEnd() const;

    const Token &next();

    bool check_valid_type(TokenType type);

    bool match_advance(TokenType type);

    const Token& consume(TokenType type, const std::string &err_msg);
};

#endif //MAREX_PARSER_HPP
