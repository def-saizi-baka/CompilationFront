#include "../include/Exception.h"

std::string Exception::what()
{
    return "[Error] line:" + std::to_string(this->line) + " info: " + this->message;
}

std::string parserException::what()
{
    return "[Parse Error] line:" + std::to_string(this->line) + " information: " + this->message;
}

std::string lexException::what()
{
    return "[Lexical Analysis Error] line:" + std::to_string(this->line) + " information: " + this->message;
}


std::string MultipleDefinitionsException::what()
{
    return  "[Multiple Definitions Error] line:" + std::to_string(this->line) + " information: " + this->message;
}

std::string UndefinedDefinitionsException::what()
{
    return  "[Undefined Definitions Error] line:" + std::to_string(this->line) + " information: " + this->message;
}

std::string ConstantModificationException::what()
{
    return  "[Constant Modification Error] line:" + std::to_string(this->line) + " information: " + this->message;
}
