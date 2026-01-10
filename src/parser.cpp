//
// Created by Omar Alispahic on 16. 11. 2025..
//
#include <string>
#include <vector>
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
    throw std::runtime_error("Line " + std::to_string(peek().token_line) + ": "+err_msg);
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

Statement *Parser::parseStatement() {
    if (match_advance(TokenType::VAR)) return parseVarDeclaration();
    if (match_advance(TokenType::PRINT)) return parsePrint();
    if (match_advance(TokenType::IF)) return parseIf();
    if (match_advance(TokenType::LOOP)) return parseLoop();

    if(check_valid_type(TokenType::IDENT)&& tokens[current_token+1].type == TokenType::ASSIGN){
        return parseAssign();
    }
    //Can add expr as stmt for function calls later

    Expr *expression = parseExpr();

    return new Print_ST(expression);
}

Statement *Parser::parseVarDeclaration() {
    const Token &varName = consume(TokenType::IDENT, "Expected variable name after 'var'!");
    consume(TokenType::ASSIGN, "Expected ':=' in assignment. ");
    Expr *value = parseExpr();
    return new VarDeclaration_ST(varName.val, value);
}

Statement *Parser::parseAssign() {
    const Token &varName = consume(TokenType::IDENT, "Expected variable to assign to!");
    consume(TokenType::ASSIGN,"Expected ':=' in assignment. ");
    Expr* value = parseExpr();
    return new Assign_ST(varName.val,value);
}

Statement *Parser::parsePrint() {
    consume(TokenType::L_PAR,"Expected '(' after print. ");
    Expr *value = parseExpr();
    consume(TokenType::R_PAR, "Expected ')' after print expression. ");
    return new Print_ST(value);
}

Statement *Parser::parseIf() {
    consume(TokenType::L_PAR, "Expected '(' after if. ");
    Expr *condition = parseExpr();
    consume(TokenType::R_PAR, "Expected ')' after condition. ");

    If_ST* if_block = new If_ST(condition);

    while(!check_valid_type(TokenType::ELSE)&& !check_valid_type(TokenType::FI)&& !isEnd()){
        Statement * statements = parseStatement();
        if_block->thenBranch.push_back(statements);
    }

    if(match_advance(TokenType::ELSE)){
        while(!check_valid_type(TokenType::FI)&& !isEnd()){
            Statement *statements = parseStatement();
            if_block->elseBranch.push_back(statements);
        }
    }
    consume(TokenType::FI, "Expected 'fi' at end of if block. ");
    return if_block;

}

Statement *Parser::parseLoop() {

    //range loop check and parse

    if (check_valid_type(TokenType::L_PAR)) {
        next();


        if (check_valid_type(TokenType::IDENT) && tokens[current_token + 1].type == TokenType::ARROW) {

            Token start_ident = next();
            consume(TokenType::ARROW, "Expected '->' in range loop. ");
            Token end_ident = consume(TokenType::IDENT, "Expected identifier after '->' in range loop. ");
            consume(TokenType::R_PAR, "Expected closing ')' bracket in loop. ");
            RangeLoop_ST *rangeLoop = new RangeLoop_ST(start_ident.val, end_ident.val);

            while (!check_valid_type(TokenType::DONE) && !isEnd()) {
                Statement *statements = parseStatement();
                rangeLoop->LoopBody.push_back(statements);
            }
            consume(TokenType::DONE, "Expected 'done' after loop body. ");
            return rangeLoop;
        }


        Statement *initial = nullptr;
        if (check_valid_type(TokenType::VAR)) {
            next();
            initial = parseVarDeclaration();
        } else if (check_valid_type(TokenType::IDENT)) Token identifier = next();

        consume(TokenType::SEMICOLON, "Expected ';' after loop initialisation. ");

        Expr *condition = parseExpr();

        consume(TokenType::SEMICOLON, "Expected ';' after loop condiiton. ");

        Statement *counter = nullptr;

        if (check_valid_type(TokenType::IDENT)) {
            counter = parseAssign();
        } else {
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
    throw std::runtime_error("Expected '(' after loop. ");
}

Expr* Parser::parseExpr() {
    return parseLogicOr();
}

Expr* Parser::parseLogicOr() {
    Expr* expr = parseLogicAnd();

    while(match_advance(TokenType::OR)){
        Expr * right = parseLogicAnd();
        expr = new BinaryExpr(expr,right,BinaryOperationType::OR);
    }
    return expr;
}

Expr* Parser::parseLogicAnd() {
    Expr* expr = parseLogicEqual();

    while(match_advance(TokenType::AND)){
        Expr * right = parseLogicEqual();
        expr = new BinaryExpr(expr,right,BinaryOperationType::AND);
    }
    return expr;
}

Expr* Parser::parseLogicEqual() {
    Expr* expr = parseCompare();

    while(match_advance(TokenType::EQUAL)){
        Expr * right = parseCompare();
        expr = new BinaryExpr(expr,right,BinaryOperationType::EQ);
    }
    while(match_advance(TokenType::NOT_EQ)){
        Expr * right = parseCompare();
        expr = new BinaryExpr(expr,right,BinaryOperationType::NOT_EQ);
    }
    return expr;
}

Expr* Parser::parseCompare() {
    Expr* expr = parseTerm();

    while(true){
        if(match_advance(TokenType::LESS)){
            Expr * right = parseTerm();
            expr = new BinaryExpr(expr,right,BinaryOperationType::LT);
        }

        if(match_advance(TokenType::GREATER)){
            Expr * right = parseTerm();
            expr = new BinaryExpr(expr,right,BinaryOperationType::GT);
        }
        else break;
    }
    return expr;
}

Expr* Parser::parseTerm()  {
    Expr* expr = parseFactor();

    while(true){
        if(match_advance(TokenType::PLUS)){
            Expr * right = parseFactor();
            expr = new BinaryExpr(expr,right,BinaryOperationType::ADD);
        }
        if(match_advance(TokenType::MINUS)){
            Expr * right = parseFactor();
            expr = new BinaryExpr(expr,right,BinaryOperationType::SUB);
        }
        else break;
    }
    return expr;
}

Expr* Parser::parseFactor() {

    Expr* expr = parseUnary();

    while(true){
        if(match_advance(TokenType::MULTIPLY)){
            Expr * right = parseUnary();
            expr = new BinaryExpr(expr,right,BinaryOperationType::MUL);
        }
        if(match_advance(TokenType::DIVISION)){
            Expr * right = parseUnary();
            expr = new BinaryExpr(expr,right,BinaryOperationType::DIV);
        }
        else break;
    }
    return expr;

}

Expr* Parser::parseUnary() {

    if(match_advance(TokenType::MINUS)){
        Expr* right = parseUnary();
        return new BinaryExpr(new NumExpr(0),right,BinaryOperationType::SUB);
    }
    return parsePrimary();
}

Expr* Parser::parsePrimary() {
    if(match_advance(TokenType::NUMBER)){
        return new NumExpr(std::stoi(previous().val));
    }

    if(match_advance(TokenType::STRING)){
        return new StringExpr(previous().val);
    }

    if(match_advance(TokenType::IDENT)){
        return new IdentExpr(previous().val);
    }

    if(match_advance(TokenType::NEWLN)){
        return new StringExpr(previous().val);
    }

    if(match_advance(TokenType::L_PAR)){
        Expr * expr = parseExpr();
        consume(TokenType::R_PAR, "Expected ')' after expression. ");
        return expr;
    }
    throw std::runtime_error("Line " + std::to_string(peek().token_line+1) + ": Expected expression");
}









