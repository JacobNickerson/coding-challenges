#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <iostream>
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

Token Lexer::generateToken(const std::string& input) const {
    if (input == "shit") {
        
    }
    return Token(TokenType::BooleanTrue, "true");
}

std::vector<Token>& Lexer::exportTokens() {
    return tokens;
}

void Lexer::readFile() {
    if (!file->is_open()) {
        state = LexerState::Invalid;
        return;
    }
    std::string content((std::istreambuf_iterator<char>(*file)), std::istreambuf_iterator<char>());
    auto end = content.end();
    for (auto it = content.begin(); it != content.end(); ++it) { // TODO: Add tokenization to each case
        switch(state) {
            // When starting, we clear all white space and then search for either an open curly bracket or square bracket
            // Any other input is invalid 
            case LexerState::Start: {
                if (!traverseWhitespace(it, end)) {
                    state = LexerState::Invalid; 
                    return;
                }
                if (*it == '{') {
                    state = LexerState::OpenCurlyBracket;
                    nestedState.push(LexerState::OpenCurlyBracket);
                }  else {
                    state = LexerState::Invalid;
                    return;
                }
                break;
            } 
            // After encountering an open curly bracket, we can search for either the start of a key or the closing curly bracket
            // Other inputs are invalid
            case LexerState::OpenCurlyBracket: {
                if (!traverseWhitespace(it, end)) {
                    state = LexerState::Invalid;
                    return;
                }
                switch (*it) {
                    case '}':
                        nestedState.pop();
                        state = (nestedState.empty()) ? LexerState::Finished : nestedState.top();
                        break;
                    case '"':
                        state = LexerState::OpenKeyQuote;
                        break;
                    default:
                        state = LexerState::Invalid;
                        return;
                }
                break;
            }
            // After encountering an open quote, we must encounter a closing quote to end the key, all values in between
            // are part of the key
            case LexerState::OpenKeyQuote: {
                auto keyEnd = it;
                while (keyEnd != end && *keyEnd != '"') {  // TODO: Handle escaped quotations
                    ++keyEnd;
                } 
                if (keyEnd == end) {
                    state = LexerState::Invalid;
                    return;
                }
                std::string key(it,end);
                std::cout << key << std::endl; // TODO: Remove debug line
                // TODO: Handle tokenization of key
                break;
            }
            // After closing a key with a closing quote, we search for a colon indicating the value associated with the key
            // Whitespace is allowed inbetween the key and the colon
            case LexerState::ClosedKeyQuote: {
                if (!traverseWhitespace(it, end) || *it != ':') {
                    state = LexerState::Invalid;
                    return;
                }
                state = LexerState::Colon;
                break;
            }
            // After a colon, we begin searching for a value. This part is unfortunately kinda complex. There are several valid things
            // that can be seen:
            // Strings: Enclosed in quotes, which indicate the start of a string
            // Numbers: These can be either ints or floats
            // Booleans: These take the form of `true` or `false`
            // Null: Represents no object, takes the form of `null`
            // Object: Another json object, the start of which is indicated by an open curly bracket `{` 
            // Array: An ordered collection of values, the values of an array can be any valid value specified in this list, starts with `[`
            case LexerState::Colon: {
                if (!traverseWhitespace(it, end)) {
                    state = LexerState::Invalid;
                    return;
                }
                switch (*it) {
                    case '"': {
                        state = LexerState::OpenValueQuote;
                        break;
                    } 
                    case '{': {
                        state = LexerState::OpenCurlyBracket;
                        nestedState.push(LexerState::OpenCurlyBracket);
                        break;
                    }
                    case '[': {
                        state = LexerState::OpenValueBracket;
                        nestedState.push(LexerState::OpenValueBracket);
                        break;
                    }
                    default: {
                        auto valueEnd = it;
                        while (valueEnd != end) { // TODO: Add some other condition for this iterator to capture the entire value
                            ++valueEnd;
                        }
                        std::string value(it,valueEnd);
                        std::cout << value << std::endl; // TODO: Remove debug print line
                        TokenType valueType = matchValue(value);
                        switch (valueType) {
                            case TokenType::NumberInt: {
                                break;
                            }
                            case TokenType::NumberFloat: {
                                break;
                            }
                            case TokenType::BooleanTrue: {
                                break;
                            }
                            case TokenType::BooleanFalse: {
                                break;
                            }
                            case TokenType::Null: {
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                    }
                }
                break;
            }
            case LexerState::OpenValueBracket: {

                break;
            }
            case LexerState::ClosedValueBracket: {

                break;
            }
            case LexerState::OpenValueQuote: {

                break;
            }
            case LexerState::ClosedValueQuote: {

                break;
            }
            case LexerState::ValueNumber: {

                break;
            }
            case LexerState::Comma: {

                break;
            }
            case LexerState::Invalid: {

                return;
            }
            case LexerState::Finished: {
                if (traverseWhitespace(it, end)) {
                    state = LexerState::Invalid;
                    return;
                }
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
    if (type != rawTokenTypes.end()) { return type->second; }
    std::regex jsonNumber("-?(?:0|[1-9]\\d*)(?:\\.\\d+)?(?:[eE][+-]?\\d+)?");
    std::smatch matches;
    if (!std::regex_match(value, jsonNumber)) {
        return TokenType::Invalid;
    }
    for (auto& match : matches) { // TODO: Parse into floats/integers
        if (match.matched) {
            std::cout << match.str() << std::endl;
        }
    }
    return TokenType::Invalid;
}