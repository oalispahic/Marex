//
// Created by Omar Alispahic on 31. 10. 2025..
//
#include <iostream>
#include <string>
#include "token_.hpp"

#ifndef MAREX_TOKEN_HPP
#define MAREX_TOKEN_HPP

std::vector<Token> tokenize(std::string &source) {
    std::vector<Token> tokens;

    int i = 0;
    while (i < source.size()) {
        char c = source[i];
        if (isspace(c)) {
            i++;
            continue;
        }

        if (c == '"') {
            i++;
            std::string str;
            bool closed = false;

            while (i < source.size()) {
                if (source[i] == '"') {
                    closed = true;
                    i++;
                    break;
                }
                str += source[i++];
            }

            if (!closed)
                tokens.push_back({TokenType::ERR, "Unterminated string"});
            else
                tokens.push_back({TokenType::STRING, str});
            continue;
        }

        if (isalpha(c) || c == '_') {        //Keyword search
            std::string val;
            while (i < source.size() && (isalnum(source[i]) || source[i] == '_')) {
                val += source[i++];
            }

            if (val == "new") tokens.push_back({TokenType::NEW, val});
            else if (val == "if") tokens.push_back({TokenType::IF, val});
            else if (val == "fi") tokens.push_back({TokenType::FI, val});
            else if (val == "else") tokens.push_back({TokenType::ELSE, val});
            else if (val == "loop") tokens.push_back({TokenType::LOOP, val});
            else if (val == "done") tokens.push_back({TokenType::DONE, val});
            else if (val == "print") tokens.push_back({TokenType::PRINT, val});
            else tokens.push_back({TokenType::IDENT, val});

            continue;
        }

        if (isdigit(c)) {         //Nums
            std::string num;
            while (isdigit(source[i])) {
                num += source[i];
                i++;
            }
            tokens.push_back({TokenType::NUMBER, num});
            continue;
        }



        if ((c == ':' && source[i + 1] == '=') || (c == '-' && source[i + 1] == '>')) {
            std::string spec;
            spec += c;
            spec += source[i + 1];
            if (spec == ":=") tokens.push_back({TokenType::ASSIGN, spec});
            if (spec == "->") tokens.push_back({TokenType::ARROW, spec});
            i += 2;
            continue;
        }

        switch (c) {
            case '+' :
                tokens.push_back({TokenType::PLUS, "+"});
                break;
            case '-' :
                tokens.push_back({TokenType::MINUS, "-"});
                break;
            case '*' :
                tokens.push_back({TokenType::MULTIPLY, "*"});
                break;
            case '/' :
                tokens.push_back({TokenType::DIVISION, "/"});
                break;
            case '<' :
                tokens.push_back({TokenType::LESS, "<"});
                break;
            case '>' :
                tokens.push_back({TokenType::GREATER, ">"});
                break;
            case '=' :
                tokens.push_back({TokenType::EQUAL, "="});
                break;
            case '&' :
                tokens.push_back({TokenType::AND, "&"});
                break;
            case '|' :
                tokens.push_back({TokenType::OR, "|"});
                break;
            case '(' :
                tokens.push_back({TokenType::L_PAR, "("});
                break;
            case ')' :
                tokens.push_back({TokenType::R_PAR, ")"});
                break;
            case ';' :
                tokens.push_back({TokenType::SEMICOLON, ";"});
                break;
            default:
                tokens.push_back({TokenType::ERR, std::string(1, c)});
                break;

        }
        i++;
    }

    tokens.push_back({TokenType::END_OF_FILE, ""});
    return tokens;

}


#endif //MAREX_TOKEN_HPP
