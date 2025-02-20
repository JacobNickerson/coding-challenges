#include "jason.hpp"
#include "token.hpp"

Jason::Jason(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

// TODO: Remove debug printlines
void Jason::processTokens() {  // processes the tokens to form a valid json
    using namespace std;
    for (const auto& token : tokens) {
        auto currTokenType = token.getType();
        std::cout << "Processing token | Lexeme: " << token.getLexeme() << " | Type: "; token.printTokenType();
        if (currTokenType == TokenType::Invalid) {
            state = JasonState::Invalid;
            cout << "encountered invalid token\n";
            continue;
        }
        cout << "Current state: ";
        switch(state) {
            case JasonState::Start: {
                cout << "start\n";
                handleStart(currTokenType);
                break;
            } 
            case JasonState::OpenCurlyBracket: {
                cout << "open curly bracket\n";
                handleOpenCurlyBracket(currTokenType);
                break;
            }
            case JasonState::Key: {
                cout << "key\n";
                handleKey(currTokenType);
                break; 
            }
            case JasonState::Colon: {
                cout << "colon\n";
                handleColon(currTokenType);
                break;
            }
            case JasonState::value: {  // TODO: This entire section needs to be verified
                cout << "value\n";
                handleValue(currTokenType);
                break;
            }
            case JasonState::OpenSquareBracket: {
                cout << "open square bracket\n";
                handleOpenSquareBracket(currTokenType);
                break;
            }
            case JasonState::Comma: { 
                cout << "comma\n";
                handleComma(currTokenType);
                break;
            }
            // UH OH!!!
            case JasonState::Invalid: {
                status = ReturnCode::SyntaxError;
                return;
            }
            // If we finish (by closing the root bracket), but encounter tokens after this, json is invalid
            case JasonState::Finished: {
                status = ReturnCode::SyntaxError;
                return;
            }
        }
    }
    status = (state == JasonState::Finished) ? ReturnCode::Valid : ReturnCode::SyntaxError;
}

// When starting, look for any primitive
void Jason::handleStart(const TokenType& currTokenType) {
    switch(currTokenType) {
        case TokenType::OpenCurlyBracket: {
            recursiveState.push(JasonState::OpenCurlyBracket);
            state = JasonState::OpenCurlyBracket;
            break;
        }
        case TokenType::OpenSquareBracket: {
            recursiveState.push(JasonState::OpenSquareBracket);
            state = JasonState::OpenSquareBracket;
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
            state = JasonState::Finished;
            break;
        }
        default: {
            state = JasonState::Invalid;
            break;
        }
    }
}
// After encountering an open curly bracket, we can search for a key or the closing curly bracket
// Other inputs are invalid
void Jason::handleOpenCurlyBracket(const TokenType& currTokenType) {
    switch (currTokenType) {
        case TokenType::ClosedCurlyBracket: {
            recursiveState.pop();
            // if stack is empty, we just closed the root object, otherwise, this object is a value
            state = (recursiveState.empty()) ? JasonState::Finished : JasonState::value;
            break;                                                                                  
        }
        case TokenType::String: {
            state = JasonState::Key;
            break;
        }
        default: {
            state = JasonState::Invalid;
            break;
        }
    }
}
// After a key we search for a colon indicating the value associated with the key
// If there is no colon, invalid
void Jason::handleKey(const TokenType& currTokenType) {
    state = (currTokenType == TokenType::Colon) ? JasonState::Colon : JasonState::Invalid;
}
// After a colon, we look for a primitive data type, there are six types
// Strings: Enclosed in quotes, which indicate the start of a string
// Numbers: These can be either ints, floats, or scientific
// Booleans: These can be either BooleanTrue or BooleanFalse
// Null: Represents no object
// Object: Another json object, the start of which is indicated by an open curly bracket `{` 
// Array: An ordered collection of values, the values of an array can be any valid value specified in this list, starts with `[`
void Jason::handleColon(const TokenType& currTokenType) {
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
            state = JasonState::value;
            break;
        }
        case TokenType::OpenCurlyBracket: {
            state = JasonState::OpenCurlyBracket;
            recursiveState.push(JasonState::OpenCurlyBracket);
            break;
        }
        case TokenType::OpenSquareBracket: {
            state = JasonState::OpenSquareBracket;
            recursiveState.push(JasonState::OpenSquareBracket);
            break;
        }
        default: {
            state = JasonState::Invalid;
            break;
        }
    }
}
// After a value, the behavior depends on our current recursiveState
// If we are inside an array, we can look for commas, denoting additional values inside the array
// or closing brackets
// If we are inside an object, we can look for commas, denoting additional key-value pairs or closing
// brackets
void Jason::handleValue(const TokenType& currTokenType) {
    if (recursiveState.empty()) {
        // This should never happen...
        std::cerr << "WTF RECURSIVESTATE IS EMPTY YO\n";
        state = JasonState::Invalid;
        return;
    }
    JasonState prevState = recursiveState.top(); 
    switch(prevState) {
        case JasonState::OpenSquareBracket: {
            switch(currTokenType) {
                case TokenType::ClosedSquareBracket: {
                    // TODO: This needs to be reviewed
                    recursiveState.pop();
                    state = (recursiveState.empty()) ? JasonState::Finished : JasonState::value;
                    break;
                }
                case TokenType::Comma: {
                    state = JasonState::Comma;
                    break;
                }
                default: { // TODO: Need to check if this is true
                    state = JasonState::Invalid;
                    break;
                }
            }
            break;
        }
        case JasonState::OpenCurlyBracket: {
            switch(currTokenType) {
                case TokenType::ClosedCurlyBracket: {
                    // TODO: This needs to be reviewed
                    recursiveState.pop();
                    // if stack is empty, we just closed the root object, otherwise, this object is a value
                    state = (recursiveState.empty()) ? JasonState::Finished : JasonState::value;
                    break;
                }
                case TokenType::Comma: {
                    state = JasonState::Comma;
                    break;
                }
                default: { // TODO: Need to check if this is true
                    state = JasonState::Invalid;
                    break;
                }
            }
            break;
        }
        default: {
            // This should not be reachable...
            std::cerr << "WTF value CRINGE\n";
            state = JasonState::Invalid;
            break;
        }
    }
}
// When we encounter an open bracket, this indicates the start of an array
// An array contains any type of valid primitive data type, delimited by commas, and is terminated by a closing bracket ']'
void Jason::handleOpenSquareBracket(const TokenType& currTokenType) {
    switch(currTokenType) {
        case TokenType::ClosedSquareBracket: {
            // TODO: This needs to be reviewed
            recursiveState.pop();
            state = (recursiveState.empty()) ? JasonState::Finished : JasonState::value;
            break;
        }
        case TokenType::OpenSquareBracket: {
            recursiveState.push(JasonState::OpenSquareBracket);
            state = JasonState::OpenSquareBracket;
            break;
        }
        case TokenType::OpenCurlyBracket: {
            recursiveState.push(JasonState::OpenCurlyBracket);
            state = JasonState::OpenCurlyBracket;
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
            state = JasonState::value;
            break;
        }
        default: {
            state = JasonState::Invalid;
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
void Jason::handleComma(const TokenType& currTokenType) {    
    if (recursiveState.empty()) {
        // This should never happen...
        std::cerr << "WTF RECURSIVESTATE IS EMPTY YO\n";
        state = JasonState::Invalid;
        return;
    }
    auto prevState = recursiveState.top();
    switch (prevState) {
        case JasonState::OpenCurlyBracket: {
            state = (currTokenType == TokenType::String) ? JasonState::Key : JasonState::Invalid;
            break;
        }
        case JasonState::OpenSquareBracket: {
            switch(currTokenType) {
                case TokenType::OpenCurlyBracket: {
                    recursiveState.push(JasonState::OpenCurlyBracket);
                    state = JasonState::OpenCurlyBracket;
                    break;
                }
                case TokenType::OpenSquareBracket: {
                    recursiveState.push(JasonState::OpenSquareBracket);
                    state = JasonState::OpenSquareBracket;
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
                    state = JasonState::value;
                    break;
                }
                default: {
                    state = JasonState::Invalid;
                    break;
                }
            }
            break;
        }
        default: {
            // Should be unreachable
            state = JasonState::Invalid;
            break;
        }
    }
}

ReturnCode Jason::getStatus() const {
    return status;
}