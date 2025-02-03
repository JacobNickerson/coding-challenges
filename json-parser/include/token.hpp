#pragma once

#include <string>

enum class TokenType {
   OpenCurlyBracket, ClosedCurlyBracket, OpenSquareBracket, ClosedSquareBracket, Quote, Comma, String, Number 
};

class Token {
    public:
        Token(TokenType type, std::string value) : type(type), value(value) {}

    private:
        TokenType type;
        std::string value;
};
