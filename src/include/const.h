#pragma once
#include <string>


namespace parser_config {
	const int ACCEPT = 10000;
	const int ERROR = -10000;
};

namespace Config {
	const std::string path_keyword = "./config/keywords.txt";
	const std::string path_operator = "./config/operator_symbol.txt";
	const std::string path_delimiter = "./config/delimiter.txt";
	const std::string path_unstop = "./config/unstop.txt";
	const std::string log_path = "./config/parser.log";
	const std::string grammar_path = "./config/grammar.txt";
	const std::string end = "$";
	const int end_int = -2;
};