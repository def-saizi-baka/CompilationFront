#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>
#include <exception>

//抽象的语法分析器的异常类
class Exception :public std::exception
{
public:
    Exception(std::string message, int line) : message(message), line(line),std::exception(){};
    virtual ~Exception() = default;
    virtual std::string what();
protected:
    std::string message;
    int line;
};

//语法分析器产生的错误
class parserException : public Exception
{
public:
    parserException(std::string message, int line) : Exception(message, line) {};
    virtual ~parserException()= default;
    virtual std::string what();
};

class lexException : public Exception
{
public:
    lexException(std::string message, int line) : Exception(message, line) {};
    virtual ~lexException()= default;
    virtual std::string what();
};


#endif /* PARSEREXCEPTION_HPP */
