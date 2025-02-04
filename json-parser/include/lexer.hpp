#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <stack>
#include <iostream> // TODO: Remove after debugging
#include "token.hpp"

enum class LexerState {
    Start, OpenCurlyBracket, OpenKeyQuote, ClosedKeyQuote, Colon, OpenValueBracket, ClosedValueBracket, OpenValueQuote,
    ClosedValueQuote, ValueNumber, Comma, Invalid, Finished 
};

class Lexer {
    public:
        Lexer(std::unique_ptr<std::fstream>& file); 

        Token generateToken(const std::string& input) const;

        // This function is named "exportTokens" because it is meant
        // to be used to give the vector tokens to a jason object, which
        // takes ownership of the vector by calling std::move in its constructor
        std::vector<Token>& exportTokens();

        // TODO: Remove after testing
        void testMatchValue(std::string value) {
            matchValue(value);
        }

        void readFile();

    private:
        LexerState state = LexerState::Start;
        std::stack<LexerState> nestedState;
        std::unique_ptr<std::fstream> file;
        std::vector<Token> tokens;
        std::unordered_map<std::string, TokenType> structuralChars;
        std::unordered_map<std::string, TokenType> rawTokenTypes;

        bool traverseWhitespace(std::string::iterator& it, const std::string::iterator& end) const;

        // TODO: Implement testing
        TokenType matchValue(const std::string& value) const;
};