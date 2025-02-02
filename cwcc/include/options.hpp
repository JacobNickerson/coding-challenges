#pragma once

#include <unordered_map>
#include <functional>
#include <string>

class options{
    public:
        options();
        void setDefault();
        bool parseFlag(std::string command); 

        void toggleBytes();
        void toggleChars();
        void toggleLines();
        void toggleMaxLineLength();
        void toggleWords();
        
        void displayHelpMenu();

        bool helpMenu = false;
        bool bytes = false;
        bool chars = false;
        bool lines = false;
        bool maxLength = false;
        bool words = false;
        std::unordered_map<std::string, std::function<void()>> commandFlags;
};
