#include "options.hpp"
#include <iostream>

options::options() {
    commandFlags = {
        {"-c", [this](){ return this->toggleBytes(); }},
        {"--bytes",[this](){ return this->toggleBytes(); }},
        {"-m",[this](){ return this->toggleChars(); }},
        {"--chars",[this](){ return this->toggleChars(); }},
        {"-l",[this](){ return this->toggleLines(); }},
        {"--lines",[this](){ return this->toggleLines(); }},
        {"-L",[this](){ return this->toggleMaxLineLength(); }},
        {"--max-line-length",[this](){ return this-> toggleMaxLineLength(); }},
        {"-w",[this](){ return this->toggleWords(); }},
        {"--words",[this](){ return this->toggleWords(); }},
        {"--help", [this](){ return this->displayHelpMenu(); }},
        {"-", [this](){ return this->toggleReadSTDIN(); }}
    };
}

OptionParseType options::parseFlag(std::string command) {
    if (command.starts_with("--files0-from=")) {
        return toggleReadFile(command);
    }
    if (commandFlags.find(command) == commandFlags.end()) {
        std::cout << "cwcc: unrecognized option '" << command << "'\n";
        std::cout << "Try cwcc --help for more information.\n";
        return OptionParseType::invalid;
    }
    return commandFlags[command]();
}

OptionParseType options::displayHelpMenu() {
    std::cout << "Usage: cwcc [OPTION]... [FILE]...\n";
    std::cout << "  or:  cwcc [OPTION]... --files0-from=F\n";
    std::cout << "Print newline, word, and byte counts for each FILE, and a total line if\n";
    std::cout << "more than one FILE is specified. A word is a nonempty sequence of non white\n";
    std::cout << "space delimited by white space characters or by start or end of input.\n";
    std::cout << std::endl;
    std::cout << "With no FILE, or when FILE is -, read standard input.\n";
    std::cout << std::endl;
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
    return OptionParseType::helpmenu;
}

void options::setDefault() {
    bytes = true;
    chars = false;
    lines = true;
    maxLength = false;
    words = true;
}

OptionParseType options::toggleBytes() {
    bytes = true;
    return OptionParseType::setting;
}

OptionParseType options::toggleChars() {
    chars = true;
    return OptionParseType::setting;
}

OptionParseType options::toggleLines() {
    lines = true;
    return OptionParseType::setting;
}

OptionParseType options::toggleReadFile(const std::string& command) {
    int ind = command.find_first_of("=");
    fileList = command.substr(ind+1, command.length());
    readFile = true;
    return OptionParseType::filename;
}

OptionParseType options::toggleMaxLineLength() {
    maxLength = true;
    return OptionParseType::setting;
}

OptionParseType options::toggleWords() {
    words = true;
    return OptionParseType::setting;
}

OptionParseType options::toggleReadSTDIN() {
    readSTDIN = true;
    maxResultWidth = 6;
    return OptionParseType::filename;
}