#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <stack>
#include "token.hpp"

enum class LexerState {
    Start, OpenCurlyBracket, ClosedCurleyBracket, OpenKeyQuote, ClosedKeyQuote, Colon,
    OpenValueBracket, ClosedValueBracket, OpenValueQuote, PostValue, Comma, Invalid, Finished 
};

class Lexer {
    public:
        Lexer(std::unique_ptr<std::fstream>& file); 

        Token generateToken(const std::string& input) const;

        // This function is named "exportTokens" because it is meant
        // to be used to give the vector tokens to a jason object, which
        // takes ownership of the vector by calling std::move in its constructor
        std::vector<Token>& exportTokens();

        void readFile();

        bool valid();

    private:
        LexerState state = LexerState::Start;
        std::stack<LexerState> recursiveState;
        std::unique_ptr<std::fstream> file;
        std::vector<Token> tokens;
        std::unordered_map<std::string, TokenType> structuralChars;
        std::unordered_map<std::string, TokenType> rawTokenTypes;

        bool traverseWhitespace(std::string::iterator& it, const std::string::iterator& end) const;

        // TODO: Implement testing
        TokenType matchValue(const std::string& value) const;
};