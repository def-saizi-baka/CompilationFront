#include "symbolTable.h"

info::info()
{
	this->type = Type::none;
	this->address = nullptr;
	//this->value = nullptr;
}

info::info(string name, Type type)
{
	// 这里解释一下value是什么意思
	// value是本符号的值，无值为null
	// 外部传入，需 new之后强转为void*  ，想不到其他办法进一步封装
	this->name = name;
	this->type = type;
	this->address = static_cast<void*>(this);//this指针强转过来
	//this->value = value;
}

info::info(const info& i)
{
	this->name = i.name;
	this->type = i.type;
	this->address = static_cast<void*>(this);//this指针强转过来
	//this->value = value;
}

info& info::operator=(const info& e)
{
	this->name = e.name;
	this->type = e.type;
	this->address = static_cast<void*>(this);//this指针强转过来
	//this->value = e.value;
	// TODO: 在此处插入 return 语句
}


symbolTable::symbolTable()
{
	
}

symbolTable::~symbolTable()
{
	for (auto& t : this->symbols) {
		delete t.second;
	}
}

bool symbolTable::insert(string name, Type type)
{
	info* pi = new info(name, type);
	int key = hashf(*pi);
	if (symbols.count(key) == 0) { //没有这个符号
		symbols[hashf(*pi)] = pi;//插入
		return true;
	}
	else
		return false;
}

int symbolTable::hashf(const info& e) const
{
	int ascii_sum = 0;
	for (const auto& i : e.name) {
		ascii_sum += i;
	}
	return ascii_sum;
}

int symbolTable::hashf(const string& e) const
{
	int ascii_sum = 0;
	for (const auto& i : e) {
		ascii_sum += i;
	}
	return ascii_sum;
}

bool symbolTable::find(string name,info& e)
{
	int key = hashf(name);
	auto iter = symbols.find(key);
	if (iter == symbols.end())//没找到
		return false;
	else {
		e = *(iter->second);//给出找到的符号信息
		return true;
	}
}

