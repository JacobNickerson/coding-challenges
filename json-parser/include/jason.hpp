#pragma once

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Jason {
    Jason() {}
    Jason(Jason* parent) : parent(parent) {}
    std::variant<std::string,
                 int,
                 float,
                 bool,
                 std::nullptr_t,
                 std::vector<std::shared_ptr<Jason>>,
                 std::unordered_map<std::string, std::shared_ptr<Jason>>> data = nullptr;
    Jason* parent = nullptr;

    void print() const;
};

// Used for type deduction in Jason methods
template <typename... Ts> 
struct Overloaded : Ts... {
    using Ts::operator()...;
};
template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;


/*
JSON primitives:
String
NumberInt
NumberFloat
NumberSci
BoolTrue
BoolFalse
Null
Object
Array


*/