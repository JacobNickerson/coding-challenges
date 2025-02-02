#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <algorithm>

Parser::Parser(std::unique_ptr<std::ifstream>& stream, std::string filename) : filename(filename), stream(std::move(stream)) {}

void Parser::parseFile() {
    if (!stream) { return; }
        std::string content((std::istreambuf_iterator<char>(*stream)), std::istreambuf_iterator<char>());
        result.bytes = content.size();
        result.chars = content.length();
        result.lines = std::count(content.begin(), content.end(), '\n');
        std::istringstream contentstream(content);
        std::string garbageCan;
        auto it = content.begin();
        auto lastBreak = content.begin();
        while ((it = std::find(it, content.end(), '\n')) != content.end()) {  //TODO: Fix max line length calculations
            size_t lineLength = std::distance(lastBreak, it);
            result.maxLength = std::max(lineLength, result.maxLength); 
            lastBreak = ++it;
        }
        while (contentstream >> garbageCan) {
            result.words++;
        }
}

void Parser::closeFile() {
    stream.reset();
}

results Parser::results() {
    return result;
}