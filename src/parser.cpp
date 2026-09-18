//
// Created by Omar Alispahic on 16. 11. 2025..
//
#include <string>
#include <vector>
#include <stdexcept>
#include "../include/token_.hpp"
#include "../include/parser.hpp"
#include "../include/ast_nodes.hpp"

const Token &Parser::peek() const { return tokens[current_token]; }

const Token &Parser::previous() const { return tokens[current_token - 1]; }

bool Parser::isEnd() const { return tokens[current_token].type == TokenType::END_OF_FILE; }

const Token &Parser::next() {
    if (!isEnd()) ++current_token;
    return previous();
}

bool Parser::check_valid_type(TokenType type) {
    if (isEnd()) return false;
    return tokens[current_token].type == type;
}

bool Parser::match_advance(TokenType type) {
    if (check_valid_type(type)) {
        next();
        return true;
    }
    return false;
}

const Token &Parser::consume(TokenType type, const std::string &err_msg) {
    if (check_valid_type(type)) return next();
    throw std::runtime_error("Line " + std::to_string(peek().token_line) + ": " + err_msg);
}


Program *Parser::parse() {
    Program *prog = new Program;
    while (!isEnd()) {
        Statement *statements = parseStatement();
        if (statements) prog->statements.push_back(statements);
        else break;
    }
    return prog;
}

ParseStatus Parser::getParseStatus(const std::vector<Token> &tokens) {
    int parenDepth = 0;
    int blockDepth = 0;

    TokenType lastSignificant = TokenType::END_OF_FILE;

    for (const auto &token: tokens) {
        if (token.type == TokenType::END_OF_FILE) break;
        if (token.type == TokenType::ERR) return ParseStatus::ERR;

        lastSignificant = token.type;

        if (token.type == TokenType::L_PAR) {
            ++parenDepth;
        } else if (token.type == TokenType::R_PAR) {
            --parenDepth;
            if (parenDepth < 0) return ParseStatus::ERR;
        } else if (token.type == TokenType::IF || token.type == TokenType::LOOP) {
            ++blockDepth;
        } else if (token.type == TokenType::FI || token.type == TokenType::DONE) {
            --blockDepth;
            if (blockDepth < 0) return ParseStatus::ERR;
        }
    }

    if (parenDepth > 0 || blockDepth > 0) return ParseStatus::WAIT;

    switch (lastSignificant) {
        case TokenType::VAR:
        case TokenType::ASSIGN:
        case TokenType::ARROW:
        case TokenType::STEP:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVISION:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::EQUAL:
        case TokenType::NOT_EQ:
        case TokenType::AND:
        case TokenType::OR:
        case TokenType::L_PAR:
        case TokenType::SEMICOLON:
            return ParseStatus::WAIT;
        default:
            break;
    }

    try {
        Parser parser(tokens);
        Program *program = parser.parse();
        delete program;
        return ParseStatus::COMPLETE;
    } catch (...) {
        return ParseStatus::ERR;
    }
}

Statement *Parser::parseStatement() {
    if (match_advance(TokenType::VAR)) return parseVarDeclaration();
    if (match_advance(TokenType::PRINT)) return parsePrint();
    if (match_advance(TokenType::IF)) return parseIf();
    if (match_advance(TokenType::LOOP)) return parseLoop();
    if (match_advance(TokenType::SYS)) return parseSystem();

    if (check_valid_type(TokenType::IDENT) && tokens[current_token + 1].type == TokenType::PLUSPLUS) {
        return parseAssign();
    }

    if (check_valid_type(TokenType::IDENT) && tokens[current_token + 1].type == TokenType::ASSIGN) {
        return parseAssign();
    }

    //Can add expr as stmt for function calls later

    Expr *expression = parseExpr();
    return new NullStmt();
}

Statement *Parser::parseVarDeclaration() {
    const Token &varName = consume(TokenType::IDENT, "Expected variable name after 'var'!");
    if (!match_advance(TokenType::ASSIGN)) {
        Expr *nullExpr = new NullExpr;
        return new VarDeclaration_ST(varName.val, nullExpr);
    }

    Expr *value = parseExpr();
    return new VarDeclaration_ST(varName.val, value);
}


Statement *Parser::parseAssign() {
    const Token &varName = consume(TokenType::IDENT, "Expected variable to assign to!");
    consume(TokenType::ASSIGN, "Expected ':=' in assignment. ");
    Expr *value = parseExpr();
    return new Assign_ST(varName.val, value);
}

Statement *Parser::parsePrint() {
    consume(TokenType::L_PAR, "Expected '(' after print. ");
    Expr *value = parseExpr();
    consume(TokenType::R_PAR, "Expected ')' after print expression. ");
    return new Print_ST(value);
}

Statement *Parser::parseSystem() {
    consume(TokenType::L_PAR, "Expected '(' after sys. ");
    Token system_statement = consume(TokenType::STRING, "Expected string as system statement. ");
    consume(TokenType::R_PAR, "Expected ')' after system statement. ");
    return new System_ST(system_statement.val);
}

Statement *Parser::parseIf() {
    consume(TokenType::L_PAR, "Expected '(' after if. ");
    Expr *condition = parseExpr();
    consume(TokenType::R_PAR, "Expected ')' after condition. ");

    If_ST *if_block = new If_ST(condition);

    while (!check_valid_type(TokenType::ELSE) && !check_valid_type(TokenType::FI) && !isEnd()) {
        Statement *statements = parseStatement();
        if_block->thenBranch.push_back(statements);
    }

    if (match_advance(TokenType::ELSE)) {
        while (!check_valid_type(TokenType::FI) && !isEnd()) {
            Statement *statements = parseStatement();
            if_block->elseBranch.push_back(statements);
        }
    }
    consume(TokenType::FI, "Expected 'fi' at end of if block. ");
    return if_block;
}

Statement *Parser::parseLoop() {
    if (check_valid_type(TokenType::L_PAR)) return parseFor();
    return parseRange();
}

Statement *Parser::parseRange() {
    Expr *start = parseRangeValue();
    if (!start) {
        throw std::runtime_error("Expected start value or variable in range loop. ");
    }

    consume(TokenType::ARROW, "Expected '->' in range loop. ");

    Expr *end = parseRangeValue();
    if (!end) {
        delete start;
        throw std::runtime_error("Expected end value or variable in range loop. ");
    }

    Expr *step = nullptr;
    if (match_advance(TokenType::STEP)) {
        step = parseRangeValue();
        if (!step) {
            delete start;
            delete end;
            throw std::runtime_error("Expected step value or variable in range loop. ");
        }
    }

    RangeLoop_ST *rangeLoop = new RangeLoop_ST(start, end, step);
    while (!check_valid_type(TokenType::DONE) && !isEnd()) {
        Statement *statements = parseStatement();
        rangeLoop->LoopBody.push_back(statements);
    }
    consume(TokenType::DONE, "Expected 'done' after loop body. ");
    return rangeLoop;
}

Expr *Parser::parseRangeValue() {
    if (check_valid_type(TokenType::IDENT)) {
        return new IdentExpr(next().val);
    }

    bool negative = false;
    if (match_advance(TokenType::MINUS)) {
        negative = true;
    }

    if (check_valid_type(TokenType::NUMBER)) {
        int parsed = std::stoi(next().val);
        return new NumExpr(negative ? -parsed : parsed);
    }
    return nullptr;
}

Statement *Parser::parseFor() {
    consume(TokenType::L_PAR, "Expected '(' after loop for c-style loop. ");

    Statement *initial = nullptr;
    if (check_valid_type(TokenType::VAR)) {
        next();
        initial = parseVarDeclaration();
    } else if (check_valid_type(TokenType::IDENT)) {
        initial = parseAssign();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after loop initialisation. ");
    Expr *condition = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after loop condition. ");

    Statement *counter = nullptr;
    if (check_valid_type(TokenType::IDENT)) {
        counter = parseAssign();
    } else {
        delete initial;
        delete condition;
        throw std::runtime_error("Expected assignment for loop increment. ");
    }

    consume(TokenType::R_PAR, "Expected closing ')' for loop header. ");
    Loop_ST *forLoop = new Loop_ST(initial, condition, counter);

    while (!check_valid_type(TokenType::DONE) && !isEnd()) {
        Statement *statement = parseStatement();
        forLoop->LoopBody.push_back(statement);
    }
    consume(TokenType::DONE, "Expected 'done' after loop body. ");
    return forLoop;
}

Expr *Parser::parseExpr() {
    return parseLogicOr();
}

Expr *Parser::parseLogicOr() {
    Expr *expr = parseLogicAnd();

    while (match_advance(TokenType::OR)) {
        Expr *right = parseLogicAnd();
        expr = new BinaryExpr(expr, right, BinaryOperationType::OR);
    }
    return expr;
}

Expr *Parser::parseLogicAnd() {
    Expr *expr = parseLogicEqual();

    while (match_advance(TokenType::AND)) {
        Expr *right = parseLogicEqual();
        expr = new BinaryExpr(expr, right, BinaryOperationType::AND);
    }
    return expr;
}

Expr *Parser::parseLogicEqual() {
    Expr *expr = parseCompare();

    while (true) {
        if (match_advance(TokenType::EQUAL)) {
            Expr *right = parseCompare();
            expr = new BinaryExpr(expr, right, BinaryOperationType::EQ);
        } else if (match_advance(TokenType::NOT_EQ)) {
            Expr *right = parseCompare();
            expr = new BinaryExpr(expr, right, BinaryOperationType::NOT_EQ);
        } else {
            return expr;
        }
    }
}

Expr *Parser::parseCompare() {
    Expr *expr = parseTerm();

    while (true) {
        if (match_advance(TokenType::LESS)) {
            Expr *right = parseTerm();
            expr = new BinaryExpr(expr, right, BinaryOperationType::LT);
        } else if (match_advance(TokenType::GREATER)) {
            Expr *right = parseTerm();
            expr = new BinaryExpr(expr, right, BinaryOperationType::GT);
        } else {
            return expr;
        }
    }
}

Expr *Parser::parseTerm() {
    Expr *expr = parseFactor();

    while (true) {
        if (match_advance(TokenType::PLUS)) {
            Expr *right = parseFactor();
            expr = new BinaryExpr(expr, right, BinaryOperationType::ADD);
        } else if (match_advance(TokenType::MINUS)) {
            Expr *right = parseFactor();
            expr = new BinaryExpr(expr, right, BinaryOperationType::SUB);
        } else {
            return expr;
        }
    }
}

Expr *Parser::parseFactor() {
    Expr *expr = parseUnary();

    while (true) {
        if (match_advance(TokenType::MULTIPLY)) {
            Expr *right = parseUnary();
            expr = new BinaryExpr(expr, right, BinaryOperationType::MUL);
        } else if (match_advance(TokenType::DIVISION)) {
            Expr *right = parseUnary();
            expr = new BinaryExpr(expr, right, BinaryOperationType::DIV);
        } else {
            return expr;
        }
    }
}

Expr *Parser::parseUnary() {
    if (match_advance(TokenType::MINUS)) {
        Expr *right = parseUnary();
        return new BinaryExpr(new NumExpr(0), right, BinaryOperationType::SUB);
    }

    return parsePrimary();
}

Expr *Parser::parsePrimary() {
    if (match_advance(TokenType::NUMBER)) {
        return new NumExpr(std::stoi(previous().val));
    }

    if (match_advance(TokenType::FLOAT)) {
        return NumExpr::makeFloat(std::stof(previous().val));
    }

    if (match_advance(TokenType::STRING)) {
        return new StringExpr(previous().val);
    }

    if (match_advance(TokenType::IDENT)) {
        return new IdentExpr(previous().val);
    }

    if (match_advance(TokenType::NEWLN)) {
        return new StringExpr(previous().val);
    }

    if (match_advance(TokenType::L_PAR)) {
        Expr *expr = parseExpr();
        consume(TokenType::R_PAR, "Expected ')' after expression. ");
        return expr;
    }
    throw std::runtime_error("Line " + std::to_string(peek().token_line + 1) + ": Expected expression");
}
