#pragma once

#include <stdexcept>
#include <string>
#include <iostream>


// todo change back to enum class
enum class TokenType {
   OpenCurlyBracket, ClosedCurlyBracket, OpenSquareBracket, ClosedSquareBracket,
   Colon, Comma, String, NumberInt, NumberFloat, NumberScientific, BooleanTrue, BooleanFalse,
   Null, Invalid
};


class Token {
    public:
        Token(TokenType type, std::string lexeme) : type(type), lexeme(lexeme) {}
        Token(TokenType type) : type(type) {
            // When given a TokenType where the lexeme is non-variable, we don't need
            // to pass the lexeme to construct the token, we can just assume what it is
            switch(type) {
                case TokenType::OpenCurlyBracket: {
                    lexeme = "{";
                    break;
                }
                case TokenType::ClosedCurlyBracket: {
                    lexeme = "}";
                    break;
                }
                case TokenType::OpenSquareBracket: {
                    lexeme = "[";
                    break;
                }
                case TokenType::ClosedSquareBracket: {
                    lexeme = "]";
                    break;
                }
                case TokenType::Colon: {
                    lexeme = ":";
                    break;
                }
                case TokenType::Comma: {
                    lexeme = ",";
                    break;
                }
                case TokenType::BooleanTrue: {
                    lexeme = "true";
                    break;
                }
                case TokenType::BooleanFalse: {
                    lexeme = "false";
                    break;
                }
                case TokenType::Null: {
                    lexeme = "null";
                    break;
                }
                default: {
                    throw std::runtime_error("Variable token created with no lexeme");
                }
            }
        }

        TokenType getType() const { return type; }

        // added for debugging
        std::string getLexeme() const { return lexeme; }
        void printTokenType() const {
            using namespace std;
            switch(type) {
                case TokenType::OpenCurlyBracket: {
                    cout << '{';
                    break;
                }
                case TokenType::ClosedCurlyBracket: {
                    cout << '}';
                    break;
                }
                case TokenType::OpenSquareBracket: {
                    cout << '[';
                    break;
                }
                case TokenType::ClosedSquareBracket: {
                    cout << ']';
                    break;
                }
                case TokenType::Colon: {
                    cout << ':';
                    break;
                }
                case TokenType::Comma: {
                    cout << ',';
                    break;
                }
                case TokenType::String: {
                    cout << "String";
                    break;
                }
                case TokenType::NumberInt: {
                    cout << "Int";
                    break;
                }
                case TokenType::NumberFloat: {
                    cout << "Float";
                    break;
                }
                case TokenType::NumberScientific: {
                    cout << "Scientific";
                    break;
                }
                case TokenType::BooleanTrue: {
                    cout << "BoolTrue";
                    break;
                }
                case TokenType::BooleanFalse: {
                    cout << "BoolFalse";
                    break;
                }
                case TokenType::Null: {
                    cout << "Null";
                    break;
                }
                case TokenType::Invalid: {
                    cout << "INVALID TOKEN";
                    break;
                }
            }
            cout << endl;
        }

    private:
        TokenType type;
        std::string lexeme; // 💅
};
