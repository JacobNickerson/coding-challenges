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
			if (arg.length() == 1) {
				filenames.push_back("\0STDIN");
				continue;
			}
			defaultSettings = false;
			if (!settings.parseFlag(arg)) {
				return 1;
			} else if (settings.helpMenu) {
				return 0;
			}
		} else {
			filenames.push_back(arg);
		}
	}
	if (defaultSettings) { settings.setDefault(); }

	if (filenames.size() <= 0) {
		results result;
		auto STDIN = std::make_unique<std::ifstream>("/dev/stdin");
		if (!STDIN->is_open()) {
			std::cerr << "error: could not read stdin\n";
			return 1;
		}
		Parser stdinparser(STDIN, "");
		stdinparser.parseFile();
		result += stdinparser.results();
		result.print(settings, "");
	} 
	if (filenames.size() > 0) {
		results total;
		for (auto& filename : filenames) {
			if (filename != "\0STDIN") {
				auto file = std::make_unique<std::ifstream>(filename);
				if (!file->is_open()) {
					std::cerr << "cwcc: " << filename << ": No such file or directory\n";
					continue;
				}
				Parser parser(file, filename);
				parser.parseFile();
				parser.results().print(settings, filename);
				total += parser.results();
			} else {
				auto STDIN = std::make_unique<std::ifstream>("/dev/stdin");
				if (!STDIN->is_open()) {
					std::cerr << "cwcc: error: could not read stdin\n";
					return 1;
				}
				Parser stdinparser(STDIN, "-");
				stdinparser.parseFile();
				stdinparser.results().print(settings, "-");
				total += stdinparser.results();
			}
		}
		total.print(settings, "total");
	}

	return 0;
}
