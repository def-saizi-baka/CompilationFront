#pragma once
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

static string::size_type min(string::size_type a, string::size_type b, string::size_type c);

enum class InputState{
	CORRECT,
    END_OF_FILE,
    NO_CLOSED_COMMENT,
    ERROR_COMMENT
};

class InputBuffer
{
public:
	InputBuffer();
	InputBuffer(const string& path);
	InputState readline();
	InputState readAll();
	bool getIsExegesis();
	int getLineNumber();
	void pop(string& s);
	void reset();
	bool open(const string& path);
	const string& getBuffer() const;
	~InputBuffer();
private:
	string buffer;
	string path;
	ifstream fin;
	int line;
	bool isExegesis;//是否在注释��??
};
