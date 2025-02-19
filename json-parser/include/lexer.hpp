#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "token.hpp"

enum class LexerStatus {
    Valid, OpenStringError, IllegalEscapeChar, UnescapedControlChar, FileNotFoundError
};

class Lexer {
    public:
        Lexer(std::unique_ptr<std::fstream>& file); 

        Token generateToken(const std::string& input) const;

        // This function returns a std::move(tokens) and is intended
        // to transfer ownership of the tokens to jason
        std::vector<Token> exportTokens();

        void readFile();

        bool valid();

    private:
        LexerStatus status = LexerStatus::Valid;
        std::unique_ptr<std::fstream> file;
        std::vector<Token> tokens;
        std::unordered_map<std::string, TokenType> structuralChars;
        std::unordered_map<std::string, TokenType> rawTokenTypes;

        bool traverseWhitespace(std::string::iterator& it, const std::string::iterator& end) const;

        std::string readString(std::string::iterator& it, const std::string::iterator& end);

        // TODO: Implement testing
        TokenType matchValue(const std::string& value) const;
};