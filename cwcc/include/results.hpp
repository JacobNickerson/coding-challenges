#pragma once

#include "options.hpp"

struct results {
    int lines{0};
    int words{0};
    int chars{0};
    int bytes{0};
    int maxLength{0};
    void print(const options& opt, std::string);
};

results operator+(const results& A, const results& B);
results operator-(const results& A, const results& B);