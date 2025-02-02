#pragma once

#include <memory>
#include "results.hpp"

class Parser {
    public:
        explicit Parser(std::unique_ptr<std::ifstream>& stream, std::string filename);
        ~Parser() = default;

        Parser(const Parser& other) = delete;
        Parser& operator=(const Parser& other) = delete;

        Parser(Parser&& other) = default;
        Parser& operator=(Parser&& other) = default;

        // Parses file for wc information, it is assumed that the object has been passed a valid
        // unique ptr that owns a file that is already open
        void parseFile(); 
        // Closes file, file is closed if open by destructor, but this can be called to close file early
        void closeFile();
        
        // Returns results member of the file, should only be called after parseFile(), otherwise
        // results will all be initialized to zero
        results results();

    private:

        std::string filename;
        std::unique_ptr<std::ifstream> stream;
        struct results result;
};