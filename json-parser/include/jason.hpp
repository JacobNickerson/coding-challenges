#pragma once

#include <vector>
#include <stack>
#include "token.hpp"

enum ReturnCode {
    Unprocessed = -1,
    Valid = 0, 
    FileNotFound = 1,
    SyntaxError = 2,
};

enum class JasonState {
    Start, OpenSquareBracket, OpenCurlyBracket, Key, Colon, value, Comma, Invalid, Finished 
};

// Taken from lexer.cpp, may or may not be useful for adapting the old lexer method
// enum class LexerStatus {
//     Start, OpenCurlyBracket, ClosedCurleyBracket, OpenKeyQuote, ClosedKeyQuote, Colon,
//     OpenValueBracket, ClosedValueBracket, OpenValueQuote, value, Comma, Invalid, Finished 
// };

class Jason {
    public:
        Jason(std::vector<Token> tokens);

        void processTokens();

        ReturnCode getStatus() const;

    private:
        std::vector<Token> tokens;
        // Used to handle the logic of nested objects
        std::stack<JasonState> recursiveState; 
        JasonState state = JasonState::Start;
        ReturnCode status = ReturnCode::Unprocessed;

        // Methods used to handle state machine of json parsing
        // It is assumed each method is called 
        void handleStart(const TokenType& currTokenType);
        void handleOpenCurlyBracket(const TokenType& currTokenType);
        void handleKey(const TokenType& currTokenType);
        void handleColon(const TokenType& currTokenType);
        void handleValue(const TokenType& currTokenType);
        void handleOpenSquareBracket(const TokenType& currTokenType);
        void handleComma(const TokenType& currTokenType);

}; 