#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "json-parser: error: no file specified\n";
		return 2;
	}
	std::ofstream testResults("test-results.txt");
	testResults << "json-parser test results\n\n";
	Lexer jsonLexer;
	for (int i{1}; i < argc; ++i) {
		std::string filename(argv[i]);
		std::fstream file(filename);
		if (!file && !file.is_open()) {
			std::cerr << "json-parser: error: " << filename << ": No such file\n";
			continue;
		}
		jsonLexer.openFile(std::move(file));
		std::cout << "parsing: " << filename << std::endl;
		jsonLexer.readFile();
		Parser jsonParser(jsonLexer.exportTokens());
		auto test = jsonParser.generate();
		test.print();
		// auto localFileName = std::string(std::find(filename.rbegin(),filename.rend(), '/').base(), filename.end()); // holee shit batman
		// testResults << filename << " | ";
		// if (jsonParser.validate()) {
		// 	std::cout << "Valid json!" << std::endl;
		// 	testResults << "Valid JSON\n";
		// 	jsonParser.reset();
		// 	Jason test = jsonParser.generate();
		// } else {
		// 	std::cout << "Invalid json..." << std::endl;
		// 	testResults << "Invalid JSON\n";
		// }
		// std::cout << "\n\n\n";
	}
	testResults.close();
	return 0;
}
