#include "symbolTable.h"

info::info()
{
	this->type = Type::none;
	this->address = nullptr;
	//this->value = nullptr;
}

info::info(string name, Type type)
{
	// �������һ��value��ʲô��˼
	// value�Ǳ����ŵ�ֵ����ֵΪnull
	// �ⲿ���룬�� new֮��ǿתΪvoid*  ���벻�������취��һ����װ
	this->name = name;
	this->type = type;
	this->address = static_cast<void*>(this);//thisָ��ǿת����
	//this->value = value;
}

info::info(const info& i)
{
	this->name = i.name;
	this->type = i.type;
	this->address = static_cast<void*>(this);//thisָ��ǿת����
	//this->value = value;
}

info& info::operator=(const info& e)
{
	this->name = e.name;
	this->type = e.type;
	this->address = static_cast<void*>(this);//thisָ��ǿת����
	//this->value = e.value;
	// TODO: �ڴ˴����� return ���
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
	if (symbols.count(key) == 0) { //û���������
		symbols[hashf(*pi)] = pi;//����
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
	if (iter == symbols.end())//û�ҵ�
		return false;
	else {
		e = *(iter->second);//�����ҵ��ķ�����Ϣ
		return true;
	}
}

