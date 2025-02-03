#include <algorithm>
#include <cctype>
#include "lexer.hpp"
#include "token.hpp"

Lexer::Lexer(std::unique_ptr<std::fstream>& file) : file(std::move(file)) {
    structuralChars = {
        {"{", TokenType::OpenCurlyBracket},
        {"}", TokenType::ClosedCurlyBracket},
        {"[", TokenType::OpenSquareBracket},
        {"]", TokenType::ClosedSquareBracket},
        {"\"", TokenType::Comma}
    };
}

Token Lexer::generateToken(const std::string& input) {
    if (structuralChars.find(input) != structuralChars.end()) {
        Token newToken(structuralChars[input], input);
        return newToken;
    } 
    if (std::all_of(input.begin(), input.end(), ::isalnum)) { // checking for numbers
        Token newToken(TokenType::Number, input);
        return newToken;
    }
    Token newToken(TokenType::String, input);
    return newToken;
}