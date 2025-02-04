#include <fstream>
#include <iostream>
#include "lexer.hpp"
#include "jason.hpp"
#include "token.hpp"

int main(int argc, char* argv[]) {
	std::string filename("Makefile");
	auto filePtr = std::make_unique<std::fstream>(filename);
	Lexer tokenGenerator(filePtr);
	for (int i{1}; i < argc; i++) {
		std::string filename(argv[i]);
		auto filePtr = std::make_unique<std::fstream>(filename);
		if (!filePtr->is_open()) {
			std::cerr << "json-parser: error: " << filename << ": No such file\n";
			return 1; 
		}
		Lexer jsonLexer(filePtr);
		jsonLexer.readFile();
		if (jsonLexer.valid()) {
			Jason lordJason(tokenGenerator.exportTokens());
		} else {
			return 2; // TODO: Implement an enum of return codes, lexer should return this
		}
	}
	return 0;
}
