#ifndef INTRECODE_H
#define INTERCODE_H

#pragma once
#include "parserTree.h"
#include <string>
#include <vector>

//#include "config.h"
using namespace std;


class InterCode;

// 四元式类
class Quadruple
{
private:
	int cur;		// 当前的代码地址
	string op;
	string left_num;
	string right_num;
	string res;
public:
	friend InterCode;
	Quadruple(const int cur, const string op, const string left_num, const string right_num, const string res);

};

// 非终结符基类
class VN
{
protected:
	int type;
public:
	VN();
	virtual vector<int>& getTrueList() = 0;
	virtual vector<int>& getFalseList() = 0;
	virtual vector<int>& getNextList() = 0;
	virtual int getNextquad() = 0;
};

//非终结符 E
class E : public VN
{
protected:
	string name;	// 对应的属性
	string value;	// 对应的值，常量则为数值，变量则为变量名，M为quad

	vector<int> truelist;
	vector<int> falselist;
	vector<int> nextlist;
public:
	friend InterCode;
	E(string name);
	E(string name, string value);

	vector<int>& getTrueList();
	vector<int>& getFalseList();
	
	vector<int>& getNextList() { 
		return this->falselist;
	}

	int getNextquad() { return 0; }
};

// 非终结符 S
class S : public VN
{
protected:
	vector<int> nextlist;
public:
	friend InterCode;

	vector<int>& getNextList();

	vector<int>& getTrueList() {
		return this->nextlist;
	}

	vector<int>& getFalseList() {
		return this->nextlist;
	}

	int getNextquad() { return 0; }
};

// 非终结符 M 
class M : public VN
{
protected:
	int quad;

public:
	friend InterCode;
	M(int quad);

	int getNextquad();

	vector<int>& getTrueList() {
		vector<int> a;
		return a;
	}

	vector<int>& getFalseList() {
		vector<int> a;
		return a;
	}

	vector<int>& getNextList() {
		vector<int> a;
		return a;
	}
};

// 非终结符 N
class N : public VN
{
protected:
	vector<int> nextlist;
public:
	friend InterCode;
	//N() {};
	vector<int>& getNextList();

	vector<int>& getTrueList() {
		vector<int> a;
		return a;
	}
	vector<int>& getFalseList() {
		vector<int> a;
		return a;
	}

	int getNextquad() { return 0; }
};


// 中间代码序列
class InterCode
{
private:
	vector<Quadruple> code;
	int nextquad;			// 当前待写地址
	vector<VN*> eleStack;     // E栈
	vector<int> operand;		// 操作数栈
public:
	InterCode();
	
	vector<int> merge(const vector<int>& L1,const vector<int>& L2); // 合并两条链
	void merge(const vector<int>& L1, const vector<int>& L2, vector<int>& L3);
	void merge(vector<vector<int> >L, vector<int>& res);
	void backpatch(vector<int>& L,int quad);
	int findByAddr(int quad);
	void emit(Quadruple temp);		// 	

	// 生成表达式
	void expression_statement(const node& root);
	// 操作表达式
	void operationStatement(const node& root);
	void defineConst();
	void defineVariable(const node& root);

	void assignStatement();

	void MStatement();	// 改写产生式后的辅助符号M
	void NStatement();	// 改写产生式后的辅助符号N

	// 逻辑表达式
	void andStatement();
	void orStatement();
	void notStatement();
	void relopStatement();

	void boolExpr();	// 此处应该特指关系表达式

	// 选择语句
	void if_then_statement();
	void if_else_statement();

	// 循环语句
	void while_do_statement();

	void genCode(const node& root);

};
#endif // !INTRERCODE_H


