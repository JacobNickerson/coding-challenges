#pragma once

#include <string>
#include <iostream>  // TODO: remove later


enum class TokenType {
   OpenCurlyBracket, ClosedCurlyBracket, OpenSquareBracket, ClosedSquareBracket,
   Colon, Comma, String, NumberInt, NumberFloat, NumberScientific, BooleanTrue, BooleanFalse,
   Null, Invalid
};


class Token {
    public:
        Token(TokenType type, std::string lexeme); 
        Token(TokenType type); 
        TokenType getType() const;

        // added for debugging
        std::string getLexeme() const;
        void printTokenType() const;

    private:
        TokenType type;
        std::string lexeme; // 💅
};
