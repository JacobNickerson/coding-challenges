#include "parser.hpp"
#include "token.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

// TODO: Remove debug printlines
void Parser::validate() {  // processes the tokens to form a valid json
    using namespace std;
    for (const auto& token : tokens) {
        auto currTokenType = token.getType();
        std::cout << "Processing token | Lexeme: " << token.getLexeme() << " | Type: "; token.printTokenType();
        if (currTokenType == TokenType::Invalid) {
            state = ParserState::Invalid;
            cout << "encountered invalid token\n";
            continue;
        }
        cout << "Current state: ";
        switch(state) {
            case ParserState::Start: {
                cout << "start\n";
                handleStart(currTokenType);
                break;
            } 
            case ParserState::OpenCurlyBracket: {
                cout << "open curly bracket\n";
                handleOpenCurlyBracket(currTokenType);
                break;
            }
            case ParserState::Key: {
                cout << "key\n";
                handleKey(currTokenType);
                break; 
            }
            case ParserState::Colon: {
                cout << "colon\n";
                handleColon(currTokenType);
                break;
            }
            case ParserState::value: {  // TODO: This entire section needs to be verified
                cout << "value\n";
                handleValue(currTokenType);
                break;
            }
            case ParserState::OpenSquareBracket: {
                cout << "open square bracket\n";
                handleOpenSquareBracket(currTokenType);
                break;
            }
            case ParserState::Comma: { 
                cout << "comma\n";
                handleComma(currTokenType);
                break;
            }
            // UH OH!!!
            case ParserState::Invalid: {
                status = ReturnCode::SyntaxError;
                return;
            }
            // If we finish (by closing the root bracket), but encounter tokens after this, json is invalid
            case ParserState::Finished: {
                status = ReturnCode::SyntaxError;
                return;
            }
        }
    }
    status = (state == ParserState::Finished) ? ReturnCode::Valid : ReturnCode::SyntaxError;
}

// When starting, look for any primitive
void Parser::handleStart(const TokenType& currTokenType) {
    switch(currTokenType) {
        case TokenType::OpenCurlyBracket: {
            recursiveState.push(ParserState::OpenCurlyBracket);
            state = ParserState::OpenCurlyBracket;
            break;
        }
        case TokenType::OpenSquareBracket: {
            recursiveState.push(ParserState::OpenSquareBracket);
            state = ParserState::OpenSquareBracket;
            break;
        }
        case TokenType::String: {
            // empty case for fallthrough
        }
        case TokenType::NumberInt: {

        }
        case TokenType::NumberFloat: {

        }
        case TokenType::NumberScientific: {

        }
        case TokenType::BooleanFalse: {

        }
        case TokenType::BooleanTrue: {

        }
        case TokenType::Null: {
            state = ParserState::Finished;
            break;
        }
        default: {
            state = ParserState::Invalid;
            break;
        }
    }
}
// After encountering an open curly bracket, we can search for a key or the closing curly bracket
// Other inputs are invalid
void Parser::handleOpenCurlyBracket(const TokenType& currTokenType) {
    switch (currTokenType) {
        case TokenType::ClosedCurlyBracket: {
            recursiveState.pop();
            // if stack is empty, we just closed the root object, otherwise, this object is a value
            state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
            break;                                                                                  
        }
        case TokenType::String: {
            state = ParserState::Key;
            break;
        }
        default: {
            state = ParserState::Invalid;
            break;
        }
    }
}
// After a key we search for a colon indicating the value associated with the key
// If there is no colon, invalid
void Parser::handleKey(const TokenType& currTokenType) {
    state = (currTokenType == TokenType::Colon) ? ParserState::Colon : ParserState::Invalid;
}
// After a colon, we look for a primitive data type, there are six types
// Strings: Enclosed in quotes, which indicate the start of a string
// Numbers: These can be either ints, floats, or scientific
// Booleans: These can be either BooleanTrue or BooleanFalse
// Null: Represents no object
// Object: Another json object, the start of which is indicated by an open curly bracket `{` 
// Array: An ordered collection of values, the values of an array can be any valid value specified in this list, starts with `[`
void Parser::handleColon(const TokenType& currTokenType) {
    switch (currTokenType) {
        case TokenType::String: {
            // empty case to use fall through
        }
        case TokenType::NumberInt: {

        }
        case TokenType::NumberFloat: {

        }
        case TokenType::NumberScientific: {

        }
        case TokenType::BooleanFalse: {

        }
        case TokenType::BooleanTrue: {

        }
        case TokenType::Null: {
            state = ParserState::value;
            break;
        }
        case TokenType::OpenCurlyBracket: {
            state = ParserState::OpenCurlyBracket;
            recursiveState.push(ParserState::OpenCurlyBracket);
            break;
        }
        case TokenType::OpenSquareBracket: {
            state = ParserState::OpenSquareBracket;
            recursiveState.push(ParserState::OpenSquareBracket);
            break;
        }
        default: {
            state = ParserState::Invalid;
            break;
        }
    }
}
// After a value, the behavior depends on our current recursiveState
// If we are inside an array, we can look for commas, denoting additional values inside the array
// or closing brackets
// If we are inside an object, we can look for commas, denoting additional key-value pairs or closing
// brackets
void Parser::handleValue(const TokenType& currTokenType) {
    if (recursiveState.empty()) {
        // This should never happen...
        std::cerr << "WTF RECURSIVESTATE IS EMPTY YO\n";
        state = ParserState::Invalid;
        return;
    }
    ParserState prevState = recursiveState.top(); 
    switch(prevState) {
        case ParserState::OpenSquareBracket: {
            switch(currTokenType) {
                case TokenType::ClosedSquareBracket: {
                    // TODO: This needs to be reviewed
                    recursiveState.pop();
                    state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
                    break;
                }
                case TokenType::Comma: {
                    state = ParserState::Comma;
                    break;
                }
                default: { // TODO: Need to check if this is true
                    state = ParserState::Invalid;
                    break;
                }
            }
            break;
        }
        case ParserState::OpenCurlyBracket: {
            switch(currTokenType) {
                case TokenType::ClosedCurlyBracket: {
                    // TODO: This needs to be reviewed
                    recursiveState.pop();
                    // if stack is empty, we just closed the root object, otherwise, this object is a value
                    state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
                    break;
                }
                case TokenType::Comma: {
                    state = ParserState::Comma;
                    break;
                }
                default: { // TODO: Need to check if this is true
                    state = ParserState::Invalid;
                    break;
                }
            }
            break;
        }
        default: {
            // This should not be reachable...
            std::cerr << "WTF value CRINGE\n";
            state = ParserState::Invalid;
            break;
        }
    }
}
// When we encounter an open bracket, this indicates the start of an array
// An array contains any type of valid primitive data type, delimited by commas, and is terminated by a closing bracket ']'
void Parser::handleOpenSquareBracket(const TokenType& currTokenType) {
    switch(currTokenType) {
        case TokenType::ClosedSquareBracket: {
            // TODO: This needs to be reviewed
            recursiveState.pop();
            state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
            break;
        }
        case TokenType::OpenSquareBracket: {
            recursiveState.push(ParserState::OpenSquareBracket);
            state = ParserState::OpenSquareBracket;
            break;
        }
        case TokenType::OpenCurlyBracket: {
            recursiveState.push(ParserState::OpenCurlyBracket);
            state = ParserState::OpenCurlyBracket;
            break;
        }
        case TokenType::String: {
            // empty case to use fall through
        }
        case TokenType::NumberInt: {

        }
        case TokenType::NumberFloat: {

        }
        case TokenType::NumberScientific: {

        }
        case TokenType::BooleanFalse: {

        }
        case TokenType::BooleanTrue: {

        }
        case TokenType::Null: {
            state = ParserState::value;
            break;
        }
        default: {
            state = ParserState::Invalid;
            break;
        }
    }
}
// When we encounter a comma, the behavior varies slightly depending on if we are in
// an array or an object
// When in an array, we search for the next value (can be any valid primitive type, including nested arrays/objects), anything else is invalid
// When in an object, we search for the next key (must be a string), anything else is invalid
// This is *almost* identical to the case of the open bracket state for each type
// However, in this next state if a closing bracket is encountered then the json is invalid
void Parser::handleComma(const TokenType& currTokenType) {    
    if (recursiveState.empty()) {
        // This should never happen...
        std::cerr << "WTF RECURSIVESTATE IS EMPTY YO\n";
        state = ParserState::Invalid;
        return;
    }
    auto prevState = recursiveState.top();
    switch (prevState) {
        case ParserState::OpenCurlyBracket: {
            state = (currTokenType == TokenType::String) ? ParserState::Key : ParserState::Invalid;
            break;
        }
        case ParserState::OpenSquareBracket: {
            switch(currTokenType) {
                case TokenType::OpenCurlyBracket: {
                    recursiveState.push(ParserState::OpenCurlyBracket);
                    state = ParserState::OpenCurlyBracket;
                    break;
                }
                case TokenType::OpenSquareBracket: {
                    recursiveState.push(ParserState::OpenSquareBracket);
                    state = ParserState::OpenSquareBracket;
                    break;
                }
                case TokenType::String: {
                    // empty case for fallthrough
                }
                case TokenType::NumberInt: {

                }
                case TokenType::NumberFloat: {

                }
                case TokenType::NumberScientific: {

                }
                case TokenType::BooleanFalse: {

                }
                case TokenType::BooleanTrue: {

                }
                case TokenType::Null: {
                    state = ParserState::value;
                    break;
                }
                default: {
                    state = ParserState::Invalid;
                    break;
                }
            }
            break;
        }
        default: {
            // Should be unreachable
            state = ParserState::Invalid;
            break;
        }
    }
}

ReturnCode Parser::getStatus() const {
    return status;
}