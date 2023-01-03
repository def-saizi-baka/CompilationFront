#ifndef INTRERCODE_H
#define INTERCODE_H

#include <string>
#include <vector>
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
	virtual vector<int>& getTrueList();
	virtual vector<int>& getFalseList();
	virtual vector<int>& getNextList();
	virtual int getNextquad();
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
	E(string);
	E(string,string);

	vector<int>& getTrueList();
	vector<int>& getFalseList();
};

// ���ս�� S
class S : public VN
{
protected:
	vector<int> nextlist;
public:
	friend InterCode;

	vector<int>& getNextList();
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
};

// ���ս�� N
class N : public VN
{
protected:
	vector<int> nextlist;
public:
	friend InterCode;
	N();
	vector<int>& getNextList();
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
	void merge(vector<const vector<int>& >L, vector<int>& res);
	void backpatch(vector<int>& L,int quad);
	int findByAddr(int quad);
	void emit(Quadruple temp);		// 	

	void defineConst();
	void defineVariable();

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

	void genCode(int reduceType, vector<E> operand);

};
#endif // !INTRERCODE_H


