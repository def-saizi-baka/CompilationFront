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
	identification, //��ʶ��
	const_number_int, //����
	const_number_double,
	const_number_char,
	const_number_str,
	keywords, //�ؼ���
	operator_symbol, //�����
	delimiter, //���
	none //Ĭ��������
};

ostream& operator<<(ostream& out, const Type t);

typedef struct {
	string name;
	string type;
	//���ﵽʱ��Ҫ�������ֵ��һ��λ��
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
	//���ﵽʱ��Ҫ�������ֵ��һ��λ��
}idDouble;

typedef struct {
	union {
		char name;
		unsigned char nameU;
		wchar_t nameW;
	}name;
	string type;
	//���ﵽʱ��Ҫ�������ֵ��һ��λ��
}idChar;

typedef struct {
	string name;
	string type;
	//���ﵽʱ��Ҫ�������ֵ��һ��λ��
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

//���Ǳ�ʶ���Ľڵ�����
class identificationInfo : public info
{
	friend class symbolTable;
private:
	map<int,id> symbols;
public:
	identificationInfo() :info(string("��ʶ��"), Type::identification) {};
	identificationInfo(const info t) :info(t) {};
	identificationInfo(const identificationInfo& t) : info(t) {};	
	int hashf(string& name);
	virtual void print()const;
	virtual bool insert(const input& t);
};

//�����Ľڵ�����

class intInfo : public info
{
	friend class symbolTable;
private:
	map<string,idInt> symbols;
public:
	string hashf(idInt);
	intInfo(const info t) :info(t) {};
	intInfo() :info("����", Type::const_number_int) {};
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
	doubleInfo() :info("������", Type::const_number_double) {};
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
	charInfo() :info("�ַ���", Type::const_number_char) {};
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
	strInfo() :info("�ַ�����", Type::const_number_str) {};
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

