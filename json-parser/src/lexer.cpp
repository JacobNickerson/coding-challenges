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

void Lexer::readFile() {              // GIANT FUNCTION
    if (!file->is_open()) {           // TODO: Should probably break this down into a couple other functions
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
                    recursiveState.push(LexerState::OpenCurlyBracket);
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
                        recursiveState.pop();
                        state = (recursiveState.empty()) ? LexerState::Finished : recursiveState.top();
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
                        recursiveState.push(LexerState::OpenCurlyBracket);
                        break;
                    }
                    case '[': {
                        state = LexerState::OpenValueBracket;
                        recursiveState.push(LexerState::OpenValueBracket);
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
                        if (valueType == TokenType::Invalid) {
                            state = LexerState::Invalid;
                            return;
                        }
                        // TODO: Make the damn token!
                    }
                }
                break;
            }
            // When we encounter an open bracket for a value, this indicates the start of an array
            // an array contains any type of valid value, delimited by commas, and is terminated by a closing bracket ']'
            case LexerState::OpenValueBracket: {
                // TODO: Handle array parsing... hoo boy
            }
            case LexerState::ClosedValueBracket: {

            }
            // After encountering a quote as a value, it is handled very similarly to a key, except it produces a 
            // String token and sets the state to a closed value quote
            case LexerState::OpenValueQuote: {
                auto valueEnd = it;
                while (valueEnd != end && *valueEnd != '"') {  // TODO: Handle escaped quotations
                    ++valueEnd;
                } 
                if (valueEnd == end) {
                    state = LexerState::Invalid;
                    return;
                }
                std::string value(it,end);
                std::cout << value << std::endl; // TODO: Remove debug line
                TokenType type = matchValue(value);
                if (type == TokenType::Invalid) {
                    state = LexerState::Invalid;
                    return;
                }
                // TODO: Handle tokenization of value
                state = LexerState::PostValue;
                break;
            }
            // After closing a value, the behavior depends on our current recursiveState
            // If we are inside an array, we can look for commas, denoting additional values inside the array
            // or closing brackets
            // If we are inside an object, we can look for commas, denoting additional key-value pairs or closing
            // brackets
            case LexerState::PostValue: {  // TODO: This entire section needs to be verified
                if (recursiveState.empty()) {
                    // This should never happen...
                    std::cerr << "WTF RECURSIVESTATE IS EMPTY YO\n";
                    state = LexerState::Invalid;
                    return;
                }
                LexerState prevState = recursiveState.top(); 
                switch(prevState) {
                    case LexerState::OpenValueBracket: {
                        if (!traverseWhitespace(it, end)) {
                            state = LexerState::Invalid;
                            return;
                        }
                        switch(*it) {
                            case ']': {
                                // TODO: Handle tokenization of arrays
                                recursiveState.pop();
                                break;
                            }
                            case ',': {
                                state = LexerState::Comma;
                                break;
                            }
                            default: { // TODO: Need to check if this is true
                                state = LexerState::Invalid;
                                return;
                            }
                        }
                        break;
                    }
                    case LexerState::OpenCurlyBracket: {
                        if (!traverseWhitespace(it, end)) {
                            state = LexerState::Invalid;
                            return;
                        }
                        switch(*it) {
                            case '}': {
                                // TODO: Handle tokenization of objects
                                recursiveState.pop();
                                break;
                            }
                            case ',': {
                                state = LexerState::Comma;
                                break;
                            }
                            default: { // TODO: Need to check if this is true
                                state = LexerState::Invalid;
                                return;
                            }
                        }
                        break;
                    }
                    default: {
                        // This should not be reachable...
                        std::cerr << "WTF POSTVALUE CRINGE\n";
                        state = LexerState::Invalid;
                        return;
                    }
                }
                break;
            }
            // Upon validly reaching the closed curly bracket state, we close an object, and return to our previous recursiveState
            // If there is no previous recursiveState, then we have finished our root object
            case LexerState::ClosedCurleyBracket: {
                recursiveState.pop();
                state = recursiveState.top();
            }
            // When we encounter a comma, the behavior varies slightly depending on if we are in
            // an array or an object
            // When in an array, we search for the next value, if we encounter closing brackets the array is invalid
            // When in an object, we search for the next key, if we encounter closing brackets the object is invalid
            // This is *almost* identical to the case of the open bracket state for each type
            // However, in this next state if a closing bracket is encountered then the json is invalid
            case LexerState::Comma: { // TODO: Handle hanging commas
                if (recursiveState.empty()) {
                    // This should never happen...
                    std::cerr << "WTF RECURSIVESTATE IS EMPTY YO\n";
                    state = LexerState::Invalid;
                    return;
                }
                state = recursiveState.top();
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

bool Lexer::valid() {
    return state == LexerState::Finished;
}