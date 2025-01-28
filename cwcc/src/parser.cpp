#include "parser.hpp"
#include <sstream>
#include <cstdio>
#include <iostream>

Parser::Parser(std::unique_ptr<std::istream> stream, std::string filename) : filename(filename), stream(std::move(stream)) {}

void Parser::parseFile() {
    if (!stream) { return; }
    if (auto check = dynamic_cast<std::fstream*>(stream.get())) { 
        if (!check->is_open()) { return; }
    }
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