#pragma once

#include "token.hpp"
#include <vector>
#include <stack>

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

    private:
        std::vector<Token> tokens;
        std::stack<JasonState> currentState;
        ReturnCode status;
}; 