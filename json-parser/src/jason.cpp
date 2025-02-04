#include "jason.hpp"

Jason::Jason(std::vector<Token>& tokens) : tokens(std::move(tokens)) {}

void Jason::processTokens() {  // processes the tokens to form a valid json

}

ReturnCode Jason::getStatus() const {
    return status;
}