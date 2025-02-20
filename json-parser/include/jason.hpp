#pragma once

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class Jason {
    public:

    private:
        std::variant<std::string,
        int,
        float,
        bool,
        std::nullptr_t,
        std::vector<std::unique_ptr<Jason>>,
        std::unordered_map<std::string,std::unique_ptr<Jason>>> data;
};


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