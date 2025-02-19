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
    Start, OpenSquareBracket, OpenCurlyBracket, Key, Colon, PostValue, Comma, Invalid, Finished 
};

// Taken from lexer.cpp, may or may not be useful for adapting the old lexer method
// enum class LexerStatus {
//     Start, OpenCurlyBracket, ClosedCurleyBracket, OpenKeyQuote, ClosedKeyQuote, Colon,
//     OpenValueBracket, ClosedValueBracket, OpenValueQuote, PostValue, Comma, Invalid, Finished 
// };

class Jason {
    public:
        Jason(std::vector<Token> tokens);

        void processTokens();

        ReturnCode getStatus() const;

        void debugPrintTokens() {
            for (const auto& token : tokens) {
                std::cout << "Token lexeme: " << token.getLexeme() << " | Token Type: "; token.printTokenType();
            }
        }

    private:
        std::vector<Token> tokens;
        // Used to handle the logic of nested objects
        std::stack<JasonState> recursiveState; 
        JasonState state = JasonState::Start;
        ReturnCode status = ReturnCode::Unprocessed;
}; 