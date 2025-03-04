#pragma once

#include "options.hpp"

struct results {
    size_t lines{0};
    size_t words{0};
    size_t chars{0};
    size_t bytes{0};
    size_t maxLength{0};
    size_t maxResultWidth{0};

    bool valid = false; // used to determine if result was successfully populated, set by parser

    void print(const options& opt, std::string filename);
    void setMaxResultWidth();

    results& operator+=(const results& other);
    results& operator-=(const results& other);
};

results operator+(const results& A, const results& B);
results operator-(const results& A, const results& B);