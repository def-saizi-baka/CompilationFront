#include "../include/symbolTable.h"

int hashf(const string& name)
{
	int res = 0;
	for (const auto& t : name)
		res += t;
	return res;
}

string get_name(type t)
{
	switch (t)
	{
	case type::BOOL:
		return "bool";
	case type::INT:
		return "int";
	case type::UNSIGNED_INT:
		return "unsigned int";
	case type::SHORT:
		return "short";
	case type::UNSIGNED_SHORT:
		return "unsigned short";
	case type::LONG:
		return "long";
	case type::UNSIGNED_LONG:
		return "unsigned long";
	case type::LONG_LONG:
		return "long long";
	case type::UNSIGNED_LONG_LONG:
		return "unsigned long long";
	case type::FLOAT:
		return "float";
	case type::DOUBLE:
		return "double";
	case type::LONG_DOUBLE:
		return "long double";
	case type::CHAR:
		return "char";
	case type::UNSIGNED_CHAR:
		return "unsigned char";
	case type::WCHAR_T:
		return "wchar_t";
	case type::STRING:
		return "string";
	case type::NONE:
		return "NULL";
	default:
		con.log("[ERROR] Fail to find the type.");
		return "[ERROR] Fail to find the type.";
	}
}

type string_type(string& s)
{
	auto iter = dic.find(s);
	if (iter == dic.end())
		return type::ERROR;
	else
		return iter->second;
}

string symbol::get_name() const
{
	return this->_name;
}

string symbol::get_type_name() const
{
	return ::get_name(_type);
}

type symbol::get_type() const
{
	return this->_type;
}

bool symbol::get_const() const
{
	return this->is_const;
}

metaSymbolTable::~metaSymbolTable()
{
	for (auto& t : table)
		if (t.second != nullptr)
			delete t.second;
}

bool metaSymbolTable::enter(string name, type t, bool is_const)
{
	symbol* s = new symbol(name, t, is_const);
	int hash = ::hashf(name);
	if (table.count(hash) > 0) {
		string message = string("[ERROR] 符号 ") + name + string(" 重定义");
		con.log(message);
		return false;
	}


	table[hash] = s;
	return true;
}

symbol* metaSymbolTable::look(string name)
{
	auto iter = table.find(::hashf(name));
	if (iter == table.end()) {
		con.log("[ERROR] 找不到符号 " + name + " ，考虑是否未定义.");
		return nullptr;
	}//找不到
	return iter->second;
}

symbolTable::symbolTable(bool is_delete)
{
	this->is_delete = is_delete;
	this->current = this->root = new node();
}

symbolTable::~symbolTable()
{
	this->root->destory(this->root);
}

bool symbolTable::enter(string name, type t, bool is_const)
{
	return this->current->table.enter(name, t, is_const);
}

symbol* symbolTable::look(string name)
{
	node* temp = current;
	symbol* res = nullptr;
	while (temp != nullptr) {
		res = temp->table.look(name);
		if (res != nullptr)
			return res;
		temp = temp->father;
	}
	return nullptr;
}

void symbolTable::mktable()
{
	if (is_delete) {
		node* temp = new node();
		temp->father = current;
		current->kids.pop_back();
		current->kids.push_back(temp);
		current = temp;
	}
	else {
		node* temp = new node();
		temp->father = current;
		current->kids.push_back(temp);
		current = temp;
	}
}

void symbolTable::detable()
{
	if (is_delete) {
		current = current->father;
		for (auto& t : current->kids)
			delete t;
	}
	else {
		this->current = current->father;
	}
}

string symbolTable::get_temp()
{
	 return temp + to_string(this->current->table.num ++); 
}
