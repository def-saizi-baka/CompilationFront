#pragma once
#include <map>
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <sstream>

using namespace std;
class symbolTable;
const string path_keyword = "./keywords.txt";
const string path_operator = "./operator_symbol.txt";
const string path_delimiter = "./delimiter.txt";

enum class Type {
	identification, //标识符
	const_number_int, //常数
	const_number_double,
	const_number_char,
	const_number_str,
	keywords, //关键字
	operator_symbol, //运算符
	delimiter, //界符
	none //默认无类型
};

ostream& operator<<(ostream& out, const Type t);

typedef struct {
	string name;
	string type;
	//这里到时候还要给具体的值留一个位置
}id;

typedef struct {
	union {
		bool nameBool;
		int nameInt;
		short nameShort;
		long nameLong;
		long long nameLongLong;
		unsigned nameUInt;
		unsigned short nameUShort;
		unsigned long nameULong;
		unsigned long long nameULongLong;
	}name;
	string type;
}idInt;

typedef struct {
	union {
		double nameD;
		float nameF;
		long double nameLD;
	}name;
	string type;
	//这里到时候还要给具体的值留一个位置
}idDouble;

typedef struct {
	union {
		char name;
		unsigned char nameU;
		wchar_t nameW;
	}name;
	string type;
	//这里到时候还要给具体的值留一个位置
}idChar;

typedef struct {
	string name;
	string type;
	//这里到时候还要给具体的值留一个位置
}idStr;


typedef struct {
	string name;
	Type Type;
	id id;
	idInt idInt;
	idDouble idDouble;
	idChar idChar;
	idStr idStr;
	string type;
}input;

class info 
{
private:
	string name;
	Type type;
public:
	friend class symbolTable;
	info();
	info(string name, Type type);
	info(const info& i);
	virtual void print()const;
	virtual bool insert(const input& t) { return true; };
};

//这是标识符的节点类型
class identificationInfo : public info
{
	friend class symbolTable;
private:
	map<int,id> symbols;
public:
	identificationInfo() :info(string("标识符"), Type::identification) {};
	identificationInfo(const info t) :info(t) {};
	identificationInfo(const identificationInfo& t) : info(t) {};	
	int hashf(string& name);
	virtual void print()const;
	virtual bool insert(const input& t);
};

//常数的节点类型

class intInfo : public info
{
	friend class symbolTable;
private:
	map<string,idInt> symbols;
public:
	string hashf(idInt);
	intInfo(const info t) :info(t) {};
	intInfo() :info("整型", Type::const_number_int) {};
	intInfo(const intInfo& t) :info(t) {};
	virtual void print()const;
	virtual bool insert(const input& t);
};


class doubleInfo : public info
{
	friend class symbolTable;
private:
	map<string,idDouble> symbols;
public:
	string hashf(idDouble);
	doubleInfo(const info t) :info(t) {};
	doubleInfo() :info("浮点型", Type::const_number_double) {};
	doubleInfo(const intInfo& t) :info(t) {};
	virtual void print()const;
	virtual bool insert(const input& t);
};


class charInfo : public info
{
	friend class symbolTable;
private:
	map<string,idChar> symbols;
public:
	string hashf(idChar);
	charInfo(const info t) :info(t) {};
	charInfo() :info("字符型", Type::const_number_char) {};
	charInfo(const intInfo& t) :info(t) {};
	virtual void print()const;
	virtual bool insert(const input& t);
};


class strInfo : public info
{
	friend class symbolTable;
private:
	map<string, idStr> symbols;
public:
	string&& hashf(idStr);
	strInfo(const info t) :info(t) {};
	strInfo() :info("字符串型", Type::const_number_str) {};
	strInfo(const intInfo& t) :info(t) {};
	virtual void print() const;
	virtual bool insert(const input& t);
};


class symbolTable
{
public:
	symbolTable();
	bool insert(const input& t);
	void print();
	~symbolTable();
private:
	map<int, info*> symbolTables;
};

