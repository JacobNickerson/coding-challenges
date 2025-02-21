#include "jason.hpp"
#include <iostream>

void Jason::print() const {
    std::visit(Overloaded{
        [](std::string s) { std::cout << s; }, 
        [](int i) { std::cout << i; },
        [](float f) { std::cout << f;},
        [](bool b) { std::cout << ((b) ? "true" : "false"); },
        [](std::nullptr_t n) { std::cout << "null"; },
        [](std::vector<std::shared_ptr<Jason>> v) {
            std::cout << "[ ";
            for (const auto& jsonPtr : v) {
                jsonPtr->print();
            }
            std::cout << " ]";
        },
        [](std::unordered_map<std::string, std::shared_ptr<Jason>> m) {
            std::cout << "{ " << std::endl;
            if (m.size() > 1) {
                auto end = std::prev(m.end());
                std::cout << "we found our end\n";
                for (auto it = m.begin(); it != end; ++it) {
                    std::cout << it->first << ": ";
                    it->second->print();
                    std::cout << ',' << std::endl;
                }
                std::cout << end->first << ": "; end->second->print();
                std::cout << "}";
            } else {
                auto key = m.begin();
                std::cout << key->first << ": ";
                key->second->print();
                std::cout << "}";
            }
        }
    }, data);
    std::cout << std::endl;
}