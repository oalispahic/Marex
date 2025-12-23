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
    int line = 0;
    while (i < source.size()) {
        char c = source[i];
        if (isspace(c)) {
            i++;
            if(c=='\n') line++;
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
                tokens.push_back({TokenType::ERR, "Unterminated string",line});
            else
                tokens.push_back({TokenType::STRING, str,line});
            continue;
        }

        if (isalpha(c) || c == '_') {        //Keyword search
            std::string val;
            while (i < source.size() && (isalnum(source[i]) || source[i] == '_')) {
                val += source[i++];
            }

            if (val == "new") tokens.push_back({TokenType::NEW, val,line});
            else if (val == "if") tokens.push_back({TokenType::IF, val,line});
            else if (val == "fi") tokens.push_back({TokenType::FI, val,line});
            else if (val == "else") tokens.push_back({TokenType::ELSE, val,line});
            else if (val == "loop") tokens.push_back({TokenType::LOOP, val,line});
            else if (val == "done") tokens.push_back({TokenType::DONE, val,line});
            else if (val == "print") tokens.push_back({TokenType::PRINT, val,line});
            else if(val == "newln") tokens.push_back({TokenType::NEWLN, "\n",line});
            else tokens.push_back({TokenType::IDENT, val,line});

            continue;
        }

        if (isdigit(c)) {         //Nums
            std::string num;
            while (isdigit(source[i])) {
                num += source[i];
                i++;
            }
            tokens.push_back({TokenType::NUMBER, num,line});
            continue;
        }


        if ((c == ':' && source[i + 1] == '=') || (c == '-' && source[i + 1] == '>') || (c=='!' && source[i+1 == '='])) {
            std::string spec;
            spec += c;
            spec += source[i + 1];
            if (spec == ":=") tokens.push_back({TokenType::ASSIGN, spec,line});
            if (spec == "->") tokens.push_back({TokenType::ARROW, spec,line});
            if (spec == "!=") tokens.push_back({TokenType::NOT_EQ, spec,line});
            i += 2;
            continue;
        }



        switch (c) {
            case '+' :
                tokens.push_back({TokenType::PLUS, "+",line});
                break;
            case '-' :
                tokens.push_back({TokenType::MINUS, "-",line});
                break;
            case '*' :
                tokens.push_back({TokenType::MULTIPLY, "*",line});
                break;
            case '/' :
                tokens.push_back({TokenType::DIVISION, "/",line});
                break;
            case '<' :
                tokens.push_back({TokenType::LESS, "<",line});
                break;
            case '>' :
                tokens.push_back({TokenType::GREATER, ">",line});
                break;
            case '=' :
                tokens.push_back({TokenType::EQUAL, "=",line});
                break;
            case '&' :
                tokens.push_back({TokenType::AND, "&",line});
                break;
            case '|' :
                tokens.push_back({TokenType::OR, "|",line});
                break;
            case '(' :
                tokens.push_back({TokenType::L_PAR, "(",line});
                break;
            case ')' :
                tokens.push_back({TokenType::R_PAR, ")",line});
                break;
            case ';' :
                tokens.push_back({TokenType::SEMICOLON, ";",line});
                break;
            default:
                tokens.push_back({TokenType::ERR, std::string(1, c),line});
                break;

        }
        i++;
    }

    tokens.push_back({TokenType::END_OF_FILE, "",line});
    return tokens;

}


#endif //MAREX_TOKEN_HPP


