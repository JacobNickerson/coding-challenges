#include "options.hpp"
#include "parser.hpp"
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
	options settings;
	std::vector<std::string> filenames;
	bool defaultSettings{true};
	for (int i{1}; i < argc; i++) {
		std::string arg(argv[i]);
		if (arg.starts_with("-")) { // command flag
			defaultSettings = false;
			if (!settings.parseFlag(arg)) {
				return 1;
			}
		} else {
			filenames.push_back(arg);
		}
	}
	if (defaultSettings) { settings.setDefault(); }

	std::vector<std::pair<std::string, std::array<int, 5>>> results;
	for (auto& filename : filenames) {
		auto file = std::make_unique<std::fstream>(filename);
		if (!file->is_open()) { std::cout << "cwcc: '" << filename << "' No such file or directory\n"; continue; }
		Parser parser(std::make_unique<std::fstream>(filename), filename);
		parser.parseFile();
		parser.printResults(settings);
	}

	if (filenames.size() <= 0) { // no files provided, read from stdin
		// this is literally just the parser methods copy and pasted
		// there has to be a better way to do this
        int lines{0};
        int words{0};
        int chars{0};
        int bytes{0};
        int maxLength{0};
	 	std::string line;
		while (std::getline(std::cin, line)) {
			bytes += line.size();
			chars += line.length();
			lines++;
			if (line.length() > maxLength) { maxLength= line.length(); }
			std::stringstream linestream(line);
			std::string garbageCan;
			while (std::getline(linestream, garbageCan, ' ')) {
				words++;
			}
		}
		std::cout << " ";
		if (settings.lines) { std::cout << lines << ' '; }
		if (settings.words) { std::cout << words << ' ';}
		if (settings.chars) { std::cout << chars << ' ';}
		if (settings.bytes) { std::cout << bytes << ' ';}
		if (settings.maxLength) { std::cout << maxLength << ' ';}
	}

	return 0;
}
