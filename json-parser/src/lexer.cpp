#include <cctype>
#include <string>
#include <fstream>
#include <regex>
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

    rawTokenTypes = {
        {"null", TokenType::Null},
        {"true", TokenType::BooleanTrue},
        {"false", TokenType::BooleanFalse}
    };
}

std::vector<Token> Lexer::exportTokens() {
    return std::move(tokens);
}

void Lexer::readFile() {              // GIANT FUNCTION
    if (!file->is_open()) {           // TODO: Should probably break this down into a couple other functions
        status = LexerStatus::FileNotFoundError;  // TODO: Part 2, need to separate this into actual lexing and parsing, right now it does both
        return;
    }
    std::string content((std::istreambuf_iterator<char>(*file)), std::istreambuf_iterator<char>());
    const auto end = content.end();
    for (auto it = content.begin(); it != content.end(); ++it) { 
        if (!traverseWhitespace(it, end)) {
            return;
        }
        switch(*it) {
            // Handle all the special characters
            case '{': {
                tokens.push_back(Token(TokenType::OpenCurlyBracket));
                break;
            }
            case '}': {
                tokens.push_back(Token(TokenType::ClosedCurlyBracket));
                break;
            }
            case '[': {
                tokens.push_back(Token(TokenType::OpenSquareBracket));
                break;
            }
            case ']': {
                tokens.push_back(Token(TokenType::ClosedSquareBracket));
                break;
            }
            case ':': {
                tokens.push_back(Token(TokenType::Colon));
                break;
            }
            case ',': {
                tokens.push_back(Token(TokenType::Comma));
                break;
            }

            // Handle strings
            case '"': {
                std::string lexeme(readString(it, end));
                std::cout << "After reading a string, it is at: " << *it << std::endl;
                if (lexeme == "\0") { return; }
                tokens.push_back(Token(TokenType::String, lexeme));
                break;
            }

            // Handle other variable data types
            default: {
                auto lexemeBegin = it;
                while (it != end && !std::isspace(*it) && *it != ',' && *it != ']' && *it != '}') {
                    ++it;
                }
                auto lexeme = std::string(lexemeBegin, it);
                tokens.push_back(Token(matchValue(lexeme), lexeme));
                --it; // decrement so we don't skip the next char
                break;
            }
        }
        auto top = tokens.back();
        top.printTokenType();
    }
}

bool Lexer::traverseWhitespace(std::string::iterator& it, const std::string::iterator& end) const {
    while (it != end && std::isspace(*it)) {
        ++it;
    }
    return it != end;
}

TokenType Lexer::matchValue(const std::string& value) const {
    auto type = rawTokenTypes.find(value);
    if (type != rawTokenTypes.end()) {
        return type->second;
    }
    std::regex jsonNumber("-?(?:0|[1-9]\\d*)(\\.\\d+)?([eE][+-]?\\d+)?");
    std::smatch matches;
    if (!std::regex_match(value, matches, jsonNumber)) {
        return TokenType::Invalid;
    }
    // matches[1] captures a decimal point
    // matches[2] captures scientific notation
    if (matches[2].matched) {
        return TokenType::NumberScientific;
    }
    if (matches[1].matched) {
        return TokenType::NumberFloat;
    }
    return TokenType::NumberInt;
}

// TODO: Move somewhere more appropriate
bool isValidUnicodeEscape(const std::string& str) {
    std::regex unicodePattern(R"(\\u[0-9A-Fa-f]{4})"); // Matches \uXXXX (4 hex digits)
    return std::regex_search(str, unicodePattern);
}
    
std::string Lexer::readString(std::string::iterator& it, const std::string::iterator& end) {
    auto strBegin = it++;
    while (it < end && *it != '"') {
        if (static_cast<unsigned char>(*it) < 0x20) {  // scuffed way of looking for illegal escape chars
            status = LexerStatus::UnescapedControlChar;
            return "\0";
        }
        if (*it == '\\') {
            ++it;
            switch(*it) {
                case '"': {
                    // empty case for fallthrough
                }
                case '\\': {

                }
                case '/': {

                }
                case 'b': {

                }
                case 'f': {

                }
                case 'n': {

                }
                case 'r': {

                }
                case 't': {
                    break;
                }
                case 'u': {
                    auto unicodeEnd = it+5;
                    if (unicodeEnd >= end) {
                        status = LexerStatus::IllegalEscapeChar;
                        return "\0";
                    }
                    std::string unicodeCode(it-1, unicodeEnd);
                    if (!isValidUnicodeEscape(unicodeCode)) { return "\0"; }
                    it = unicodeEnd-1;
                    break;
                }
                default: {
                    status = LexerStatus::IllegalEscapeChar;
                    return "\0";
                }
            }
        } 
        ++it; 
    }
    if (it >= end) {
        status = LexerStatus::OpenStringError; 
        return "\0";
    }
    return std::string(strBegin,it+1);
}

bool Lexer::valid() {
    return status == LexerStatus::Valid;
}