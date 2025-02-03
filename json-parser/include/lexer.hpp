#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "token.hpp"

class Lexer {
    public:
        Lexer(std::unique_ptr<std::fstream>& file); 

        Token generateToken(const std::string& input);

        // This function is named "exportTokens" because it is meant
        // to be used to give the vector tokens to a jason object, which
        // takes ownership of the vector by calling std::move in its constructor
        std::vector<Token>& exportTokens();

        void readFile();

    private:
        std::unique_ptr<std::fstream> file;
        std::vector<Token> tokens;
        std::unordered_map<std::string, TokenType> structuralChars;
};