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
	none //Ĭ��������
};


typedef struct info{
	string name;
	Type type;
	void* address;//�����ڵ�ǰ���ű�֮�еĵ�ַ
	//void* value;//����ֻ�ܱ�¶��ȥ�ˣ�ʵ���벻����ô��װ
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
	int hashf(const info& e) const;//������ʱ��ʵ��һ��hash����
	int hashf(const string& e) const;
	bool find(string name,info& e);
private:
	map<int, info*> symbols;
};

