#pragma once

#include <unordered_map>
#include <vector>
#include <fstream>
#include "token.hpp"

enum class LexerStatus {
    Valid, OpenStringError, IllegalEscapeChar, UnescapedControlChar, FileNotFoundError
};

class Lexer {
    public:
        Lexer();
        Lexer(std::fstream file); 

        Token generateToken(const std::string& input) const;

        // This function returns a std::move(tokens) and is intended
        // to transfer ownership of the tokens to jason
        std::vector<Token> exportTokens();

        void readFile();

        bool valid() const;

        // It's assumed that this is called after the tokens have been exported and the tokens vector is empty.
        // Checks it the newFile is valid and open, if so closes the old file, changes ownership to the new file and returns true.
        // Otherwise, returns false and keeps ownership of the old file, without closing.
        bool openFile(std::fstream newFile);

    private:
        LexerStatus status = LexerStatus::Valid;
        std::fstream file;
        std::vector<Token> tokens;
        std::unordered_map<std::string, TokenType> rawTokenTypes;

        bool traverseWhitespace(std::string::iterator& it, const std::string::iterator& end) const;

        std::string readString(std::string::iterator& it, const std::string::iterator& end);

        TokenType matchValue(const std::string& value) const;
};

bool isValidUnicodeEscape(const std::string& str);