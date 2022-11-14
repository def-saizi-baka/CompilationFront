#pragma once
#ifndef CONST_H
#define CONST_H
#include <string>


namespace parser_config {
	const int ACCEPT = 10000;
	const int ERROR = -10000;
};

namespace Config {
	const std::string end = "$";
	const int end_int = -2;
};

#endif // CONST_H