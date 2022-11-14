#include "Exception.h"

std::string Exception::what()
{
    return "[error] line:" + std::to_string(this->line) + " info: " + this->message;
}

std::string parserException::what()
{
    return "[parse error] line:" + std::to_string(this->line) + " information: " + this->message;
}

std::string lexException::what()
{
    return "[lexical analysis error] line:" + std::to_string(this->line) + " information: " + this->message;
}
