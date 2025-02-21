#pragma once

#include <vector>
#include <stack>
#include "token.hpp"
#include "jason.hpp"

enum ReturnCode {
    Unprocessed = -1,
    Valid = 0, 
    FileNotFound = 1,
    SyntaxError = 2,
};

enum class ParserState {
    Start, OpenSquareBracket, OpenCurlyBracket, Key, Colon, value, Comma, Invalid, Finished 
};

// Taken from lexer.cpp, may or may not be useful for adapting the old lexer method
// enum class LexerStatus {
//     Start, OpenCurlyBracket, ClosedCurleyBracket, OpenKeyQuote, ClosedKeyQuote, Colon,
//     OpenValueBracket, ClosedValueBracket, OpenValueQuote, value, Comma, Invalid, Finished 
// };

class Parser {
    public:
        Parser(std::vector<Token> tokens);

        // Returns a bool indicating if json is valid
        bool validate();

        // Returns a Jason object representing the json in memory
        // Will return a Jason object with no value if json is invalid
        Jason generate();

        void reset() {
            std::stack<ParserState> newStack;
            recursiveState.swap(newStack);
            state = ParserState::Start;
            status = ReturnCode::Unprocessed;
            lastKey = "";
        }

        // Can be used to see specific return codes in case of invalid json
        ReturnCode getStatus() const;

    private:
        std::vector<Token> tokens;
        // Used to handle the logic of nested objects
        std::stack<ParserState> recursiveState; 
        ParserState state = ParserState::Start;
        ReturnCode status = ReturnCode::Unprocessed;
        std::string lastKey;

        // Methods used to handle state machine of json parsing
        // Takes a pointer to the current object in order to insert into a json being constructed
        // Should be passed a nullptr in case of validation, in which case the json construction is ignored
        void handleStart(const Token& currToken, Jason*& currObj);
        void handleOpenCurlyBracket(const Token& currToken, Jason*& currObj);
        void handleKey(const Token& currToken);
        void handleColon(const Token& currToken, Jason*& currObj);
        void handleValue(const Token& currToken, Jason*& currObj);
        void handleOpenSquareBracket(const Token& currToken, Jason*& currObj);
        void handleComma(const Token& currToken, Jason*& currObj);

}; 