#include "include/lexer.hpp"
#include <iostream>
#include <fstream>

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

int Lexer::index() {
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

        if (current() == '"') {
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
                tokens.push_back({TokenType::ERR, "Unterminated string", line});
            else
                tokens.push_back({TokenType::STRING, str, line});
            continue;
        }

        if (isalpha(current()) || current() == '_') {        //Keyword search
            std::string val;
            while (index() < file.size() && (isalnum(current()) || current() == '_')) {
                val += current();
                consume();
            }

            if (val == "new") tokens.push_back({TokenType::NEW, val, line});
            else if (val == "if") tokens.push_back({TokenType::IF, val, line});
            else if (val == "fi") tokens.push_back({TokenType::FI, val, line});
            else if (val == "else") tokens.push_back({TokenType::ELSE, val, line});
            else if (val == "loop") tokens.push_back({TokenType::LOOP, val, line});
            else if (val == "done") tokens.push_back({TokenType::DONE, val, line});
            else if (val == "print") tokens.push_back({TokenType::PRINT, val, line});
            else if (val == "newln") tokens.push_back({TokenType::NEWLN, "\n", line});
            else tokens.push_back({TokenType::IDENT, val, line});

            continue;
        }

        if (isdigit(current())) {         //Nums
            std::string num;
            while (isdigit(current())) {
                num += current();
                consume();
            }
            tokens.push_back({TokenType::NUMBER, num, line});
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
            default:
                tokens.push_back({TokenType::ERR, std::string(1, current()), line});
                break;

        }
        consume();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;

}




