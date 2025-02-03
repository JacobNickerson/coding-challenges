#include "lexer.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
	for (int i{1}; i < argc; i++) {
		std::string filename(argv[i]);
		auto filePtr = std::make_unique<std::fstream>(filename);
		if (!filePtr->is_open()) {
			std::cerr << "json-parser: error: " << filename << ": No such file\n";
			return 1; 
		}
		Lexer tokenGenerator(filePtr);
		tokenGenerator.exportTokens();
	}
	return 0;
}
