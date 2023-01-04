#pragma once
#include "config.h"
#include <map>
 
extern config con;
enum class type;
class symbol;
class metaSymbolTable;
class symbolTable;
const string temp = "T";

enum class type {
	BOOL,
	INT, UNSIGNED_INT, SHORT, UNSIGNED_SHORT, LONG, UNSIGNED_LONG, LONG_LONG, UNSIGNED_LONG_LONG,
	FLOAT, DOUBLE, LONG_DOUBLE,
	CHAR, UNSIGNED_CHAR, WCHAR_T,
	STRING,
	NONE,
	ERROR
};

const std::map<string, type> dic = {
	make_pair("bool",type::BOOL),
	make_pair("int",type::INT),
	make_pair("unsigned int",type::UNSIGNED_INT),
	make_pair("short",type::SHORT),
	make_pair("unsigned short",type::UNSIGNED_SHORT),
	make_pair("long",type::LONG),
	make_pair("unsigned long",type::UNSIGNED_LONG),
	make_pair("long long",type::LONG_LONG),
	make_pair("unsigned long long",type::UNSIGNED_LONG_LONG),
	make_pair("char",type::CHAR),
	make_pair("unsigned char",type::UNSIGNED_CHAR),
	make_pair("wchar_t",type::WCHAR_T),
	make_pair("string",type::STRING),
	make_pair("NULL",type::NONE)
};

class symbol
{
public:
	symbol(string name, type t, bool is_const) :_name(name), _type(t), is_const(is_const) {};
	string get_name() const;
	string get_type_name() const;
	type get_type() const;
	bool get_const() const;
private:
	string _name;
	type _type = type::ERROR;
	bool is_const = false;
};

class metaSymbolTable
{
public:
	~metaSymbolTable();
	bool enter(string name, type t, bool is_const);
	symbol* look(string name);
private:
	map<int, symbol*> table;
	int num = 0;
	friend class symbolTable;
};

class symbolTable
{
public:
	symbolTable(bool is_delete = true);
	~symbolTable();
	bool enter(string name, type t, bool is_const);
	symbol* look(string name);
	void mktable();
	void detable();
	string get_temp();
private:
	bool is_delete;
	struct node {
		metaSymbolTable table;
		node* father = nullptr;
		vector<node*> kids;

		void destory(node* t) {
			if (t == nullptr)
				return;
			for (auto& item : t->kids) {
				destory(item);
			}
			delete t;
		}
	};
	node* current;
	node* root;
};

static int hashf(const string& name);
string get_name(type t);
type string_type(string& s);