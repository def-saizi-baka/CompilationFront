#pragma once
#include "config.h"

extern config con;
enum class type;
class symbol;
class metaSymbolTable;
class symbolTable;

enum class type {
	BOOL,
	INT, UNSIGNED_INT, SHORT, UNSIGNED_SHORT, LONG, UNSIGNED_LONG, LONG_LONG, UNSIGNED_LONG_LONG,
	FLOAT, DOUBLE, LONG_DOUBLE,
	CHAR, UNSIGNED_CHAR, WCHAR_T,
	STRING,
	NONE,
	ERROR
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