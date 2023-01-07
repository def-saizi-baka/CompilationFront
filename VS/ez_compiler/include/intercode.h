#ifndef INTRECODE_H
#define INTERCODE_H

#pragma once
#include "parserTree.h"
#include "Exception.h" 
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
	friend ostream& operator<<(ostream& out, const Quadruple& t);
	friend ofstream& operator<<(ofstream& out, const Quadruple& t);
};

// 非终结符基类
class VN
{
public:
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
	int nextquad;
public:
	friend InterCode;
	S(int nextquad = 0);

	vector<int>& getNextList();

	vector<int>& getTrueList() {
		return this->nextlist;
	}

	vector<int>& getFalseList() {
		return this->nextlist;
	}

	int getNextquad() { return this->nextquad; }
};

// 非终结符 M 
class M : public VN
{
protected:
	int quad;
	vector<int> null_vec;	// 仅为虚函数返回补充接口，无意义
public:
	friend InterCode;
	M(int quad);

	int getNextquad();

	vector<int>& getTrueList() {
		return null_vec;
	}

	vector<int>& getFalseList() {
		return null_vec;
	}

	vector<int>& getNextList() {
		return null_vec;
	}
};

// 非终结符 N
class N : public VN
{
protected:
	vector<int> nextlist;
	vector<int> null_vec;	// 仅为虚函数返回补充接口，无意义

public:
	friend InterCode;
	//N() {};
	vector<int>& getNextList();

	vector<int>& getTrueList() {
		return null_vec;
	}
	vector<int>& getFalseList() {
		return null_vec;
	}

	int getNextquad() { return 0; }
};


// 中间代码序列
class InterCode
{
private:
	vector<Quadruple> code;
	int nextquad;				// 当前待写地址
	vector<VN*> eleStack;		// E栈
	vector<int> operand;		// 操作数栈
	int line;					// 当前对应的输入代码行
	parserTree* pst;			// 引用传入
public:
	InterCode();

	InterCode(parserTree& pst);
	
	vector<int> merge(const vector<int>& L1,const vector<int>& L2); // 合并两条链
	void merge(const vector<int>& L1, const vector<int>& L2, vector<int>& L3);
	void merge(vector<vector<int> >L, vector<int>& res);
	void backpatch(vector<int>& L,int quad);
	int findByAddr(int quad);
	void emit(Quadruple temp);		// 	

	// 表达式翻译
	void allExpression(const node& root,bool bool_flag = false);

	// 操作表达式
	void operationExpression(const node& root);

	// 布尔表达式
	// void bool_expression(const node& root);
	
	// 赋值表达式
	void assignExpression(const node& root, bool bool_flag = false);

	// 计算表达式
	void calExpression(const node& root, bool bool_flag = false);

	// 逻辑表达式
	void logicExpression(string logicOp, bool bool_flag = false);
	void andExpression(bool bool_flag = false);
	void orExpression(bool bool_flag = false);
	void notExpression(bool bool_flag = false);

	// 关系表达式
	void relopExpression(const node& root, bool bool_flag = false);

	// 自增自减表达式
	void selfChangeExpression(const node& root,bool is_front);

	// 变量定义语句
	void defineConst(const node& root);
	void defineVariable(const node& root);

	// 表达式语句
	void expression2statment();

	void MStatement();	// 改写产生式后的辅助符号M
	void NStatement();	// 改写产生式后的辅助符号N

	// 变量名 -> 表达式
	void valNameExpression(const node& root,bool bool_flag = false);

	// 选择语句
	void if_then_statement();
	void if_else_statement();

	// 循环语句
	void while_do_statement();

	// 生成语句List
	void statemmentList(const node& root);

	// 生成语句块
	void statementBlock(const node& root);

	void genCode(const node& root,int line);

	void outputCode(const char* filename=NULL);
	
	
};
#endif // !INTRERCODE_H


