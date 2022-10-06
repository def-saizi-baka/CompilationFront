#pragma once
#include <map>
#include <string>


using namespace std;

enum class Type{
	keywords,
	identification,
	const_number,
	operator_symbol,
	delimiter,
	none //默认无类型
};


typedef struct info{
	string name;
	Type type;
	void* address;//符号在当前符号表之中的地址
	//void* value;//这里只能暴露出去了，实在想不好怎么包装
	info();
	info(string name, Type type);
	info(const info& i);
	info& operator=(const info& e);
}info;


class symbolTable
{
public:
	symbolTable();
	~symbolTable();
	bool insert(string name, Type type);
	int hashf(const info& e) const;//这里暂时先实现一个hash函数
	int hashf(const string& e) const;
	bool find(string name,info& e);
private:
	map<int, info*> symbols;
};

