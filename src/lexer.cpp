#include "../include/lexer.hpp"
#include <cctype>

const char *tokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::VAR: return "VAR";
        case TokenType::PRINT: return "PRINT";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FI: return "FI";
        case TokenType::LOOP: return "LOOP";
        case TokenType::DONE: return "DONE";
        case TokenType::STEP: return "STEP";
        case TokenType::FUN: return "FUN";
        case TokenType::RET: return "RET";
        case TokenType::SYS: return "SYS";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::ARROW: return "ARROW";
        case TokenType::PLUS: return "PLUS";
        case TokenType::PLUSPLUS: return "PLUSPLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVISION: return "DIVISION";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQ: return "NOT_EQ";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::L_PAR: return "L_PAR";
        case TokenType::R_PAR: return "R_PAR";
        case TokenType::L_BRACKET: return "L_BRACKET";
        case TokenType::R_BRACKET: return "R_BRACKET";
        case TokenType::DOT: return "DOT";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::NEWLN: return "NEWLN";
        case TokenType::IDENT: return "IDENT";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::STRING: return "STRING";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERR: return "ERR";
    }
    return "UNKNOWN";
}

char Lexer::peek() {
    if (curr + 1 >= file.size()) return '\0';
    return file[curr + 1];
}

char Lexer::current() {
    return file[curr];
}

void Lexer::consume() {
    curr++;
}

size_t Lexer::index() {
    return curr;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;


    while (index() < file.size()) {

        if (isspace(current())) {
            if (current() == '\n') line++;
            consume();
            continue;
        }

        if (current() == '/' && peek() == '/') {          // line comment
            while (index() < file.size() && current() != '\n') consume();
            continue;
        }

        if (current() == '/' && peek() == '*') {          // block comment
            const int start_line = line;
            consume();
            consume();
            bool closed = false;
            while (index() < file.size()) {
                if (current() == '*' && peek() == '/') {
                    consume();
                    consume();
                    closed = true;
                    break;
                }
                if (current() == '\n') line++;
                consume();
            }
            if (!closed) {
                open_block_comment = true;
                tokens.push_back({TokenType::ERR, "Unterminated block comment", start_line});
            }
            continue;
        }

        if (current() == '"') {
            const int start_line = line;
            consume();
            std::string str;
            bool closed = false;

            while (index() < file.size()) {
                if (current() == '"') {
                    closed = true;
                    consume();
                    break;
                }
                if (current() == '\n') line++;
                str += current();
                consume();
            }

            if (!closed)
                tokens.push_back({TokenType::ERR, "Unterminated string", start_line});
            else
                tokens.push_back({TokenType::STRING, str, start_line});
            continue;
        }

        if (isalpha(current()) || current() == '_') {        //Keyword search
            std::string val;
            while (index() < file.size() && (isalnum(current()) || current() == '_')) {
                val += current();
                consume();
            }

            if (val == "var") tokens.push_back({TokenType::VAR, val, line});
            else if (val == "if") tokens.push_back({TokenType::IF, val, line});
            else if (val == "fi") tokens.push_back({TokenType::FI, val, line});
            else if (val == "else") tokens.push_back({TokenType::ELSE, val, line});
            else if (val == "loop") tokens.push_back({TokenType::LOOP, val, line});
            else if (val == "done") tokens.push_back({TokenType::DONE, val, line});
            else if (val == "print") tokens.push_back({TokenType::PRINT, val, line});
            else if (val == "newln") tokens.push_back({TokenType::NEWLN, "\n", line});
            else if (val == "sys") tokens.push_back({TokenType::SYS, val, line});
            else if (val == "step") tokens.push_back({TokenType::STEP, val, line});
            else if (val == "fun") tokens.push_back({TokenType::FUN, val, line});
            else if (val == "ret") tokens.push_back({TokenType::RET, val, line});
            else tokens.push_back({TokenType::IDENT, val, line});

            continue;
        }

        if (isdigit(current())) {         //Nums
            bool isFloat = false;
            std::string num;
            while (isdigit(current())) {
                num += current();
                consume();
            }

            if (current() == '.' && isdigit(peek())) {
                isFloat = true;
                num+=current();
                consume();
                while (isdigit(current())) {
                    num+=current();
                    consume();
                }
            }

            if (isFloat) tokens.push_back({TokenType::FLOAT, num, line});
            else {
                tokens.push_back({TokenType::NUMBER, num, line});
            }
            continue;
        }


        if ((current() == ':' && peek() == '=')) {
            consume();
            consume();
            tokens.push_back({TokenType::ASSIGN, ":=", line});
            continue;
        }

        if (current() == '-' && peek() == '>') {
            consume();
            consume();
            tokens.push_back({TokenType::ARROW, "->", line});
            continue;
        }

        if (current() == '!' && peek() == '=') {
            consume();
            consume();
            tokens.push_back({TokenType::NOT_EQ, "!=", line});
            continue;
        }

        if (current() == '+' && peek() == '+') {
            consume();
            consume();
            tokens.push_back({TokenType::PLUSPLUS, "++", line});
            continue;
        }


        switch (current()) {
            case '+' :
                tokens.push_back({TokenType::PLUS, "+", line});
                break;
            case '-' :
                tokens.push_back({TokenType::MINUS, "-", line});
                break;
            case '*' :
                tokens.push_back({TokenType::MULTIPLY, "*", line});
                break;
            case '/' :
                tokens.push_back({TokenType::DIVISION, "/", line});
                break;
            case '<' :
                tokens.push_back({TokenType::LESS, "<", line});
                break;
            case '>' :
                tokens.push_back({TokenType::GREATER, ">", line});
                break;
            case '=' :
                tokens.push_back({TokenType::EQUAL, "=", line});
                break;
            case '&' :
                tokens.push_back({TokenType::AND, "&", line});
                break;
            case '|' :
                tokens.push_back({TokenType::OR, "|", line});
                break;
            case '(' :
                tokens.push_back({TokenType::L_PAR, "(", line});
                break;
            case ')' :
                tokens.push_back({TokenType::R_PAR, ")", line});
                break;
            case ';' :
                tokens.push_back({TokenType::SEMICOLON, ";", line});
                break;
            case '[':
                tokens.push_back({TokenType::L_BRACKET, "[", line});
                break;
            case ']':
                tokens.push_back({TokenType::R_BRACKET, "]", line});
                break;
            case '.':
                tokens.push_back({TokenType::DOT, ".", line});
                break;
            case ',':
                tokens.push_back({TokenType::COMMA, ",", line});
                break;
            default:
                tokens.push_back({TokenType::ERR, "Unexpected character '" + std::string(1, current()) + "'", line});
                break;

        }
        consume();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;

}




