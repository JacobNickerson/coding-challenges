#include "parser.hpp"
#include "token.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

// TODO: Remove debug printlines
bool Parser::validate() {  // processes the tokens to form a valid json
    using namespace std;
    Jason* throwaway = nullptr;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::Invalid) {
            state = ParserState::Invalid;
            cout << "encountered invalid token\n";
            continue;
        }
        cout << "Current state: ";
        switch(state) {
            case ParserState::Start: {
                cout << "start\n";
                handleStart(token, throwaway);
                break;
            } 
            case ParserState::OpenCurlyBracket: {
                cout << "open curly bracket\n";
                handleOpenCurlyBracket(token, throwaway);
                break;
            }
            case ParserState::Key: {
                cout << "key\n";
                handleKey(token);
                break; 
            }
            case ParserState::Colon: {
                cout << "colon\n";
                handleColon(token, throwaway);
                break;
            }
            case ParserState::value: {  // TODO: This entire section needs to be verified
                cout << "value\n";
                handleValue(token, throwaway);
                break;
            }
            case ParserState::OpenSquareBracket: {
                cout << "open square bracket\n";
                handleOpenSquareBracket(token, throwaway);
                break;
            }
            case ParserState::Comma: { 
                cout << "comma\n";
                handleComma(token, throwaway);
                break;
            }
            // UH OH!!!
            case ParserState::Invalid: {
                status = ReturnCode::SyntaxError;
                return false;
            }
            // If we finish (by closing the root bracket), but encounter tokens after this, json is invalid
            case ParserState::Finished: {
                status = ReturnCode::SyntaxError;
                return false;
            }
        }
        std::cout << "Processing token | Lexeme: " << token.getLexeme() << " | Type: "; token.printTokenType();
    }
    status = (state == ParserState::Finished) ? ReturnCode::Valid : ReturnCode::SyntaxError;
    return (status == ReturnCode::Valid) ? true : false;
}

Jason Parser::generate() {
    Jason root;
    Jason* curr = &root;
    for (const auto& token : tokens) {
        if (token.getType() == TokenType::Invalid) {
            state = ParserState::Invalid;
            continue;
        }
        switch(state) {
            case ParserState::Start: {
                std::cout << "Start" << '\n';
                handleStart(token, curr);
                break;
            } 
            case ParserState::OpenCurlyBracket: {
                std::cout << "OpenCurly" << '\n';
                handleOpenCurlyBracket(token, curr);
                break;
            }
            case ParserState::Key: {
                std::cout << "Key" << '\n';
                handleKey(token);
                break; 
            }
            case ParserState::Colon: {
                std::cout << "Colon" << '\n';
                handleColon(token, curr);
                break;
            }
            case ParserState::value: {  // TODO: This entire section needs to be verified
                std::cout << "Value" << '\n';
                handleValue(token, curr);
                break;
            }
            case ParserState::OpenSquareBracket: {
                std::cout << "OpenSquareBracket" << '\n';
                handleOpenSquareBracket(token, curr);
                break;
            }
            case ParserState::Comma: { 
                std::cout << "Comma" << '\n';
                handleComma(token,curr);
                break;
            }
            // UH OH!!!
            case ParserState::Invalid: {
                status = ReturnCode::SyntaxError;
                return Jason{};
            }
            // If we finish (by closing the root bracket), but encounter tokens after this, invalid
            case ParserState::Finished: {
                status = ReturnCode::SyntaxError;
                return Jason{};
            }
        }
        token.printTokenType();
    }
    status = (state == ParserState::Finished) ? ReturnCode::Valid : ReturnCode::SyntaxError;
    return (status == ReturnCode::Valid) ? std::move(root) : std::move(root); 
}

ReturnCode Parser::getStatus() const {
    return status;
}

// --==Private Methods==--

// When starting, look for any primitive
void Parser::handleStart(const Token& currToken, Jason*& currObj) {
    auto currTokenType = currToken.getType();
    switch(currTokenType) {
        case TokenType::OpenCurlyBracket: {
            recursiveState.push(ParserState::OpenCurlyBracket);
            state = ParserState::OpenCurlyBracket;
            if (currObj) {
                currObj->data = std::unordered_map<std::string, std::shared_ptr<Jason>>{};
            }
            break;
        }
        case TokenType::OpenSquareBracket: {
            recursiveState.push(ParserState::OpenSquareBracket);
            state = ParserState::OpenSquareBracket;
            if (currObj) {
                currObj->data = std::vector<std::shared_ptr<Jason>>{};
            }
            break;
        }
        case TokenType::String: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = currToken.getLexeme(); 
            }
            break;
        }
        case TokenType::NumberInt: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = std::stoi(currToken.getLexeme()); 
            }
            break;
        }
        case TokenType::NumberFloat: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = std::stof(currToken.getLexeme()); 
            }
            break;
        }
        case TokenType::NumberScientific: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = std::stof(currToken.getLexeme());
            }
            break;
        }
        case TokenType::BooleanFalse: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = false;
            }
            break;
        }
        case TokenType::BooleanTrue: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = true; 
            }
            break;
        }
        case TokenType::Null: {
            state = ParserState::Finished;
            if (currObj) {
                currObj->data = nullptr; 
            }
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
void Parser::handleOpenCurlyBracket(const Token& currToken, Jason*& currObj) {
    auto currTokenType = currToken.getType();
    switch (currTokenType) {
        case TokenType::ClosedCurlyBracket: {
            recursiveState.pop();
            // if stack is empty, we just closed the root object, otherwise, this object is a value
            state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
            if (currObj) {
                currObj = currObj->parent;
            }
            break;                                                                                  
        }
        case TokenType::String: {
            state = ParserState::Key;
            if (currObj) { if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenCurlyBracket()\n");
                } else {
                    lastKey = currToken.getLexeme();
                }
            }
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
void Parser::handleKey(const Token& currToken) {
    state = (currToken.getType() == TokenType::Colon) ? ParserState::Colon : ParserState::Invalid;
}
// After a colon, we look for a primitive data type, there are six types
// Strings: Enclosed in quotes, which indicate the start of a string
// Numbers: These can be either ints, floats, or scientific
// Booleans: These can be either BooleanTrue or BooleanFalse
// Null: Represents no object
// Object: Another json object, the start of which is indicated by an open curly bracket `{` 
// Array: An ordered collection of values, the values of an array can be any valid value specified in this list, starts with `[`
void Parser::handleColon(const Token& currToken, Jason*& currObj) {
    auto currTokenType = currToken.getType();
    switch (currTokenType) {
        case TokenType::String: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    // Verbose but we're just taking the map stored in our curr jason and creating a new jason to map the key to with a parent pointer pointing to our curr jason
                    // then setting the new jason's value to our current token's lexeme
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = currToken.getLexeme();
                }
            }
            break;
        }
        case TokenType::NumberInt: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = std::stoi(currToken.getLexeme());
                }
            }
            break;
        }
        case TokenType::NumberFloat: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = std::stof(currToken.getLexeme());
                }
            }
            break;
        }
        case TokenType::NumberScientific: {
            // TODO: Add some sort of flag to save state as a scientific number
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = std::stof(currToken.getLexeme());
                }
            }
            break;
        }
        case TokenType::BooleanFalse: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = false;
                }
            }
            break;
        }
        case TokenType::BooleanTrue: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = true;
                }
            }
            break;
        }
        case TokenType::Null: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = nullptr;
                }
            }
            break;
        }
        case TokenType::OpenCurlyBracket: {
            state = ParserState::OpenCurlyBracket;
            recursiveState.push(ParserState::OpenCurlyBracket);
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = std::unordered_map<std::string, std::shared_ptr<Jason>>{};
                    // Set the new current object to be the object that this key refers to
                    currObj = objPairs[lastKey].get();
                }
            }
            break;
        }
        case TokenType::OpenSquareBracket: {
            state = ParserState::OpenSquareBracket;
            recursiveState.push(ParserState::OpenSquareBracket);
            if (currObj) {
                if (!std::holds_alternative<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleColon()\n");
                } else {
                    auto& objPairs = std::get<std::unordered_map<std::string, std::shared_ptr<Jason>>>(currObj->data);
                    objPairs[lastKey] = std::make_shared<Jason>(Jason(currObj)); 
                    objPairs[lastKey]->data = std::vector<std::shared_ptr<Jason>>{};
                    // Set the new current object to be the array that this key refers to
                    currObj = objPairs[lastKey].get();
                }
            }
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
void Parser::handleValue(const Token& currToken, Jason*& currObj) {
    auto currTokenType = currToken.getType(); 
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
                    recursiveState.pop();
                    state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
                    // Ending the array means that our current object reverts back to the object that contains the array
                    if (currObj) { currObj = currObj->parent; }
                    break;
                }
                case TokenType::Comma: {
                    state = ParserState::Comma;
                    break;
                }
                default: { 
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
                    if (currObj) { currObj = currObj->parent; }
                    break;
                }
                case TokenType::Comma: {
                    state = ParserState::Comma;
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
            // This should not be reachable...
            std::cerr << "WTF value CRINGE\n";
            state = ParserState::Invalid;
            break;
        }
    }
}
// When we encounter an open bracket, this indicates the start of an array
// An array contains any type of valid data type, delimited by commas, and is terminated by a closing bracket ']'
void Parser::handleOpenSquareBracket(const Token& currToken, Jason*& currObj) {
    auto currTokenType = currToken.getType(); 
    switch(currTokenType) {
        case TokenType::ClosedSquareBracket: {
            recursiveState.pop();
            state = (recursiveState.empty()) ? ParserState::Finished : ParserState::value;
            if (currObj) { currObj = currObj->parent; }
            break;
        }
        case TokenType::OpenSquareBracket: {
            recursiveState.push(ParserState::OpenSquareBracket);
            state = ParserState::OpenSquareBracket;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = std::vector<std::shared_ptr<Jason>>{};
                    // Set the new current object to be the new array that was pushed into the current array
                    currObj = arr.back().get(); 
                }
            }
            break;
        }
        case TokenType::OpenCurlyBracket: {
            recursiveState.push(ParserState::OpenCurlyBracket);
            state = ParserState::OpenCurlyBracket;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = std::unordered_map<std::string, std::shared_ptr<Jason>>{};  
                    // Set the new current object to be the new map that was pushed into the current array
                    currObj = arr.back().get(); 
                }
            }
            break;
        }
        case TokenType::String: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = currToken.getLexeme(); 
                }
            }
            break;
        }
        case TokenType::NumberInt: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                            std::cout << "Lexeme: " << currToken.getLexeme() << std::endl;
                    arr.back()->data = std::stoi(currToken.getLexeme()); 
                }
            }
            break;
        }
        case TokenType::NumberFloat: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = std::stof(currToken.getLexeme()); 
                }
            }
            break;
        }
        case TokenType::NumberScientific: {
            // TODO: Add some type of flag to save scientific state
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = std::stof(currToken.getLexeme()); 
                }
            }
            break;
        }
        case TokenType::BooleanFalse: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = false; 
                }
            }
            break;
        }
        case TokenType::BooleanTrue: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = true; 
                }
            }
            break;
        }
        case TokenType::Null: {
            state = ParserState::value;
            if (currObj) {
                if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                    throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                } else {
                    auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                    arr.push_back(std::make_shared<Jason>(currObj));
                    arr.back()->data = nullptr; 
                }
            }
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
// When in an object, we search for the next key (must be a string), anything else is invalid
// When in an array, we search for the next value (can be any valid primitive type, including nested arrays/objects), anything else is invalid
// This is *almost* identical to the case of the open bracket state for each type
// However, in this next state if a closing bracket is encountered then the json is invalid
// It might be possible to avoid this function entirely and change the open bracket functions to handle a flag indicating a comma
// However, I ain't doin that :cryinglaughing:
void Parser::handleComma(const Token& currToken, Jason*& currObj) {    
    auto currTokenType = currToken.getType();
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
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = std::unordered_map<std::string, std::shared_ptr<Jason>>();  
                            // Set the new current object to be the new map that was pushed into the current array
                            currObj = arr.back().get(); 
                        }
                    }
                    break;
                }
                case TokenType::OpenSquareBracket: {
                    recursiveState.push(ParserState::OpenSquareBracket);
                    state = ParserState::OpenSquareBracket;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = std::vector<std::shared_ptr<Jason>>();  
                            // Set the new current object to be the new array that was pushed into the current array
                            currObj = arr.back().get(); 
                        }
                    }
                    break;
                }
                case TokenType::String: {
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = currToken.getLexeme();  
                        }
                    }
                    break;
                }
                case TokenType::NumberInt: {
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            std::cout << "Lexeme: " << currToken.getLexeme() << std::endl;
                            arr.back()->data = std::stoi(currToken.getLexeme());  
                        }
                    }
                }
                case TokenType::NumberFloat: {
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = std::stof(currToken.getLexeme());  
                        }
                    }
                }
                case TokenType::NumberScientific: {
                    // TODO: Add sci flag
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = std::stof(currToken.getLexeme());  
                        }
                    }
                }
                case TokenType::BooleanFalse: {
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = false; 
                        }
                    }
                }
                case TokenType::BooleanTrue: {
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = true; 
                        }
                    }
                }
                case TokenType::Null: {
                    state = ParserState::value;
                    if (currObj) {
                        if (!std::holds_alternative<std::vector<std::shared_ptr<Jason>>>(currObj->data)) {
                            throw std::runtime_error("STATE MACHINE BROKE IN handleOpenSquareBracket()\n");
                        } else {
                            auto& arr = std::get<std::vector<std::shared_ptr<Jason>>>(currObj->data);
                            arr.push_back(std::make_shared<Jason>(currObj));
                            arr.back()->data = nullptr;  
                        }
                    }
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