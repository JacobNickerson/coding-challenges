#pragma once

#include <string>

enum class TokenType {
   OpenCurlyBracket, ClosedCurlyBracket, OpenSquareBracket, ClosedSquareBracket, Quote, Colon, Comma, String, NumberInt, NumberFloat, NumberScientific,
   BooleanTrue, BooleanFalse, Null, Invalid
};

class Token {
    public:
        Token(TokenType type, std::string lexeme) : type(type), lexeme(lexeme) {}

    private:
        TokenType type;
        std::string lexeme; // 💅
};
