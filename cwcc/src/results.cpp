#include <iostream>
#include "results.hpp"
#include <iomanip>

results& results::operator+=(const results& other) {
    this->lines += other.lines; 
    this->words += other.words; 
    this->chars += other.chars; 
    this->bytes += other.bytes; 
    this->maxLength += other.maxLength; 
    this->maxResultWidth = std::max(this->maxResultWidth, other.maxResultWidth);
    return *this;
}
results& results::operator-=(const results& other) {
    this->lines -= other.lines; 
    this->words -= other.words; 
    this->chars -= other.chars; 
    this->bytes -= other.bytes; 
    this->maxLength -= other.maxLength; 
    this->maxResultWidth = std::max(this->maxResultWidth, other.maxResultWidth);
    return *this;
}
results operator+(const results& A, const results& B) {
    results temp(A.lines+B.lines, A.words+B.words, A.chars+B.chars, A.bytes+B.bytes, A.maxLength+B.maxLength);
    temp.setMaxResultWidth();
    return temp;
}
results operator-(const results& A, const results& B) {
    results temp(A.lines-B.lines, A.words-B.words, A.chars-B.chars, A.bytes-B.bytes, A.maxLength-B.maxLength);
    temp.setMaxResultWidth();
    return temp;
}

void results::print(const options& opt, std::string filename) {
    if (opt.lines) { std::cout << std::setw(opt.maxResultWidth) << std::right << lines << ' '; }
    if (opt.words) { std::cout << std::setw(opt.maxResultWidth) << std::right << words << ' '; }
    if (opt.chars) { std::cout << std::setw(opt.maxResultWidth) << std::right << chars << ' '; }
    if (opt.bytes) { std::cout << std::setw(opt.maxResultWidth) << std::right << bytes << ' '; }
    if (opt.maxLength) { std::cout << std::setw(opt.maxResultWidth) << std::right << maxLength << ' '; }
    std::cout << std::left << filename << std::endl;
}

void results::setMaxResultWidth() {
    if (lines > 0) {
        size_t curr{0};
        size_t temp = lines;
        while (temp > 0) {
            temp /= 10;
            curr++;
        }
        maxResultWidth = std::max(curr, maxResultWidth);
    }
    if (words) {
        size_t curr{0};
        size_t temp = words;
        while (temp > 0) {
            temp /= 10;
            curr++;
        }
        maxResultWidth = std::max(curr, maxResultWidth);
    }
    if (chars) {
        size_t curr{0};
        size_t temp = chars;
        while (temp > 0) {
            temp /= 10;
            curr++;
        }
        maxResultWidth = std::max(curr, maxResultWidth);
    }
    if (bytes) {
        size_t curr{0};
        size_t temp = bytes;
        while (temp > 0) {
            temp /= 10;
            curr++;
        }
        maxResultWidth = std::max(curr, maxResultWidth);
    }
    if (maxLength) {
        size_t curr{0};
        size_t temp = maxLength;
        while (temp > 0) {
            temp /= 10;
            curr++;
        }
        maxResultWidth = std::max(curr, maxResultWidth);
    }
}