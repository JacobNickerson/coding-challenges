#include "options.hpp"
#include <iostream>

options::options() {
    commandFlags = {
        {"-c", [this](){ this->toggleBytes(); }},
        {"--bytes",[this](){ this->toggleBytes(); }},
        {"-m",[this](){ this->toggleChars(); }},
        {"--chars",[this](){ this->toggleChars(); }},
        {"-l",[this](){ this->toggleLines(); }},
        {"--lines",[this](){ this->toggleLines(); }},
        {"-L",[this](){ this->toggleMaxLineLength(); }},
        {"--max-line-length",[this](){ this-> toggleMaxLineLength(); }},
        {"-w",[this](){ this->toggleWords(); }},
        {"--words",[this](){ this->toggleWords(); }},
        {"--help", [this](){ this->displayHelpMenu(); }}
    };
}

bool options::parseFlag(std::string command) {
    if (commandFlags.find(command) == commandFlags.end()) {
        std::cout << "cwcc: unrecognized option '" << command << "'\n";
        std::cout << "Try cwcc --help for more information.\n";
        return false;
    }
    commandFlags[command]();
    return true;
}

void options::displayHelpMenu() {
    std::cout << "Usage: cwcc [OPTION]... [FILE]...\n";
    std::cout << "  or:  cwcc [OPTION]... --files0-from=F\n";
    std::cout << "Print newline, word, and byte counts for each FILE, and a total line if\n";
    std::cout << "more than one FILE is specified. A word is a nonempty sequence of non white\n";
    std::cout << "space delimited by white space characters or by start or end of input.\n";
    std::cout << std::endl;
    std::cout << "With no FILE, or when FILE is -, read standard input.\n";
    std::cout << std::endl;
    std::cout << "With no FILE, or when FILE is -, read standard input.\n";
    std::cout << "The options below may be used to select which counts are printed, always in\n";
    std::cout << "the following order: newline, word, character, byte, maximum line length.\n";
    std::cout << "  -c, --bytes            print the byte counts\n";
    std::cout << "  -m, --chars            print the character counts\n";
    std::cout << "  -l, --lines            print the newline counts\n";
    std::cout << "      --files0-from=F    read input from the files specified by\n";
    std::cout << "                          NUL-terminated names in file F;\n";
    std::cout << "                          If F is - then read names from standard input\n";   
    std::cout << "  -L, --max-line-length  print the maximum display width\n";
    std::cout << "  -w, --words            print the word counts\n";
    std::cout << "      --help             display this help and exit\n";
    helpMenu = true;
}

void options::setDefault() {
    bytes = true;
    chars = false;
    lines = true;
    maxLength = false;
    words = true;
}

void options::toggleBytes() {
    bytes = true;
}

void options::toggleChars() {
    chars = true;
}

void options::toggleLines() {
    lines = true;
}

void options::toggleMaxLineLength() {
    maxLength = true;
}

void options::toggleWords() {
    words = true;
}