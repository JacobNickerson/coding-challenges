#include <cctype>
#include <string>
#include <regex>
#include "lexer.hpp"
#include "token.hpp"

Lexer::Lexer() : rawTokenTypes({
    {"null", TokenType::Null},
    {"true", TokenType::BooleanTrue},
    {"false", TokenType::BooleanFalse}
    }) {}
Lexer::Lexer(std::fstream newFile) : Lexer() {
    file = std::move(newFile);
}

std::vector<Token> Lexer::exportTokens() {
    return std::move(tokens);
}

void Lexer::readFile() {              
    if (!file.is_open()) {           
        status = LexerStatus::FileNotFoundError;  
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
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

bool Lexer::valid() const {
    return status == LexerStatus::Valid;
}

bool Lexer::openFile(std::fstream newFile) {
    if (!newFile || !newFile.is_open()) { return false; }
    if (file && file.is_open()) { file.close(); }
    file = std::move(newFile);
    return true;
}