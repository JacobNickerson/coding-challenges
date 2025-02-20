#include <fstream>
#include "lexer.hpp"
#include "jason.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "json-parser: error: no file specified\n";
		return 2;
	}
	std::ofstream testResults("test-results.txt");
	testResults << "json-parser test results\n\n";
	for (int i{1}; i < argc; ++i) {
		std::string filename(argv[i]);
		std::fstream file(filename);
		if (!file && !file.is_open()) {
			std::cerr << "json-parser: error: " << filename << ": No such file\n";
			continue;
		}
		Lexer jsonLexer(std::move(file));
		std::cout << "parsing: " << filename << std::endl;
		jsonLexer.readFile();
		// if (!jsonLexer.valid()) { continue; }
		Jason jsonParser(jsonLexer.exportTokens());
		jsonParser.processTokens();
		// auto localFileName = std::string(std::find(filename.rbegin(),filename.rend(), '/').base(), filename.end()); // holee shit batman
		testResults << filename << " | ";
		if (jsonParser.getStatus() == ReturnCode::Valid) {
			std::cout << "Valid json!" << std::endl;
			testResults << "Valid JSON\n";
		} else {
			std::cout << "Invalid json..." << std::endl;
			testResults << "Invalid JSON\n";
		}
		std::cout << "\n\n\n";
	}
	testResults.close();
	return 0;
}
