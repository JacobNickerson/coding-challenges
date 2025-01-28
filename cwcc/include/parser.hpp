#pragma once

#include <memory>
#include "results.hpp"

class Parser {
    public:
        explicit Parser(std::unique_ptr<std::istream> stream, std::string filename);
        ~Parser() = default;

        Parser(const Parser& other) = delete;
        Parser& operator=(const Parser& other) = delete;

        Parser(Parser&& other) = default;
        Parser& operator=(Parser&& other) = default;

        void parseFile(); 
        void closeFile();

        results results();

    private:

        std::string filename;
        std::unique_ptr<std::istream> stream;
        struct results result;
};