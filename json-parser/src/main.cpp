#include <fstream>
#include <iostream>
#include "lexer.hpp"
#include "jason.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "json-parser: error: No file specified\n";
		return 2;
	}
	std::string filename(argv[1]);
	auto filePtr = std::make_unique<std::fstream>(filename);
	if (!filePtr->is_open()) {
		std::cerr << "json-parser: error: " << filename << ": No such file\n";
		return 1; 
	}
	Lexer jsonLexer(filePtr);
	jsonLexer.readFile();
	if (jsonLexer.valid()) {
		Jason lordJason(jsonLexer.exportTokens());
		lordJason.processTokens();
		return lordJason.getStatus();
	} else {
		return 3; // TODO: Implement an enum of return codes, lexer should return this
	}
	return 2; // Should be unreachable
}
