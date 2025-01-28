#include <iostream>
#include "results.hpp"

results operator+(const results& A, const results& B) {
    return results(A.lines+B.lines, A.words+B.words, A.chars+B.chars, A.bytes+B.bytes, A.maxLength+B.maxLength);
}
results operator-(const results& A, const results& B) {
    return results(A.lines-B.lines, A.words-B.words, A.chars-B.chars, A.bytes-B.bytes, A.maxLength-B.maxLength);
}

void results::print(const options& opt, std::string filename) {
    std::cout << " ";
    if (opt.lines) { std::cout << lines << ' '; }
    if (opt.words) { std::cout << words << ' ';}
    if (opt.chars) { std::cout << chars << ' ';}
    if (opt.bytes) { std::cout << bytes << ' ';}
    if (opt.maxLength) { std::cout << maxLength << ' ';}
    std::cout << filename << std::endl;
}