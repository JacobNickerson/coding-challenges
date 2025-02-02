#pragma once

#include "options.hpp"

struct results {
    size_t lines{0};
    size_t words{0};
    size_t chars{0};
    size_t bytes{0};
    size_t maxLength{0};
    int maxLineLength{0};
    void print(const options& opt, std::string filename);

    results& operator+=(const results& other);
    results& operator-=(const results& other);
};

results operator+(const results& A, const results& B);
results operator-(const results& A, const results& B);