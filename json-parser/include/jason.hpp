#pragma once

#include "token.hpp"
#include <vector>

enum ReturnCode {
    Unprocessed = -1,
    Valid = 0, 
    FileNotFound = 1,
    SyntaxError = 2,
};

enum class JasonState {
    OpenSquareBracket, OpenCurlyBracket, OpenQuote
};

class Jason {
    public:
        Jason(std::vector<Token>& tokens);

        void processTokens();

        ReturnCode getStatus() const;

    private:
        std::vector<Token> tokens;
        JasonState state;
        ReturnCode status = ReturnCode::Unprocessed;
}; 