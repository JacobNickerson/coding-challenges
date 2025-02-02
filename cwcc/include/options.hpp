#pragma once

#include <unordered_map>
#include <functional>
#include <string>

enum class OptionParseType {
    setting, invalid, filename, helpmenu
};

class options{
    public:
        options();
        void setDefault();
        OptionParseType parseFlag(std::string command); 

        OptionParseType toggleBytes();
        OptionParseType toggleChars();
        OptionParseType toggleLines();
        OptionParseType toggleReadFile(const std::string& command);
        OptionParseType toggleMaxLineLength();
        OptionParseType toggleWords();
        OptionParseType toggleReadSTDIN();
        OptionParseType displayHelpMenu();

        bool helpMenu = false;
        bool bytes = false;
        bool chars = false;
        bool lines = false;
        bool readFile = false;
        bool maxLength = false;
        bool words = false;
        bool readSTDIN = false;

        int maxResultWidth{0};

        std::string fileList = "";
        std::unordered_map<std::string, std::function<OptionParseType()>> commandFlags;
};
