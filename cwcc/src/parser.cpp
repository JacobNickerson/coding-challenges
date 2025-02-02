#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>

Parser::Parser(std::unique_ptr<std::ifstream>& stream, std::string filename) : filename(filename), stream(std::move(stream)) {}

void Parser::parseFile() {
    if (!stream) { return; }
    std::string line;
    while (std::getline(*stream, line)) {
        result.bytes += line.size();
        result.chars += line.length();
        result.lines++;
        if (line.length() > result.maxLength) { result.maxLength = line.length(); }
        std::stringstream linestream(line);
        std::string garbageCan;
        while (std::getline(linestream, garbageCan, ' ')) {
            result.words++;
        }
    }
}

void Parser::closeFile() {
    stream.reset();
}

results Parser::results() {
    return result;
}