#ifndef INTRECODE_H
#define INTERCODE_H

#pragma once
#include "parserTree.h"
#include <string>
#include <vector>

//#include "config.h"
using namespace std;


class InterCode;

// ��Ԫʽ��
class Quadruple
{
private:
	int cur;		// ��ǰ�Ĵ����ַ
	string op;
	string left_num;
	string right_num;
	string res;
public:
	friend InterCode;
	Quadruple(const int cur, const string op, const string left_num, const string right_num, const string res);

};

// ���ս������
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

//���ս�� E
class E : public VN
{
protected:
	string name;	// ��Ӧ������
	string value;	// ��Ӧ��ֵ��������Ϊ��ֵ��������Ϊ��������MΪquad

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

// ���ս�� S
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

// ���ս�� M 
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

// ���ս�� N
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


// �м��������
class InterCode
{
private:
	vector<Quadruple> code;
	int nextquad;			// ��ǰ��д��ַ
	vector<VN*> eleStack;     // Eջ
	vector<int> operand;		// ������ջ
public:
	InterCode();
	
	vector<int> merge(const vector<int>& L1,const vector<int>& L2); // �ϲ�������
	void merge(const vector<int>& L1, const vector<int>& L2, vector<int>& L3);
	void merge(vector<vector<int> >L, vector<int>& res);
	void backpatch(vector<int>& L,int quad);
	int findByAddr(int quad);
	void emit(Quadruple temp);		// 	

	// ���ɱ��ʽ
	void expression_statement(const node& root);
	// �������ʽ
	void operationStatement(const node& root);
	void defineConst();
	void defineVariable(const node& root);

	void assignStatement();

	void MStatement();	// ��д����ʽ��ĸ�������M
	void NStatement();	// ��д����ʽ��ĸ�������N

	// �߼����ʽ
	void andStatement();
	void orStatement();
	void notStatement();
	void relopStatement();

	void boolExpr();	// �˴�Ӧ����ָ��ϵ���ʽ

	// ѡ�����
	void if_then_statement();
	void if_else_statement();

	// ѭ�����
	void while_do_statement();

	void genCode(const node& root);

};
#endif // !INTRERCODE_H


