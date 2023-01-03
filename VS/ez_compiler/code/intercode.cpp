#include "../include/intercode.h"
#include <iostream>
#include <sstream>


// 此处宏定义没有用，单纯分工合作的时候，我先填上去
// 归约的种类应该从Parse类中读取静态变量
#define DEFINE_CONST 0
#define DEFINE_VARIABLE 1
#define ASSIGNMENT 2
#define BOOL_EXPR 3

#define NOT_FOUND_INDEX -1

// 四元式类
Quadruple::Quadruple(const int cur,const string op, const string left_num, const string right_num, const string res)
{
	this->cur = cur;
	this->op = op;
	this->left_num = left_num;
	this->right_num = right_num;
	this->res = res;
}


VN::VN()
{
	this->type = 0;
}

// 表达式元素
E::E(string name):VN()
{
	this->name = name;
	this->value = value;
}

// 返回真链
vector<int>& E::getTrueList()
{
	return this->truelist;
}

// 返回假链
vector<int>& E::getFalseList()
{
	return this->falselist;
}

// 非终结符 M 
M::M(int quad) :VN()
{
	this->quad = quad;
}

// 返回quad属性
int M::getNextquad()
{
	return this->quad;
}

// 返回 S.nextlist
vector<int>& S::getNextList()
{
	return this->nextlist;
}

// 返回 N.nextlist
vector<int>& N::getNextList()
{
	return this->nextlist;
}

// 中间代码
InterCode::InterCode()
{
	this->nextquad = 100;	//起始待写地址设置为100
}

// 合并TrueList、FalseList
vector<int> InterCode::merge(const vector<int>&L1,const vector<int>& L2)
{
	vector<int> temp;
	temp.insert(temp.end(), L1.begin(), L1.end());
	temp.insert(temp.end(), L2.begin(), L2.end());
	return temp;
}


// L3 = L1 + L2
void InterCode::merge(const vector<int>& L1, const vector<int>& L2, vector<int>& L3)
{
	if (L3.size() != 0)
	{
		cout << "may error" << endl;
	}
	L3.insert(L3.end(), L1.begin(), L1.end());
	L3.insert(L3.end(), L2.begin(), L2.end());
}

void InterCode::merge(vector<vector<int> >L, vector<int>& res)
{
	if (res.size() != 0)
	{
		cout << "may error" << endl;
	}
	
	for (uint32_t i=0; i<L.size(); i++)
	{
		res.insert(res.end(), L[i].begin(), L[i].end());
	}
}


// 根据地址找到对应的四元式
int InterCode::findByAddr(int quad)
{
	int index = 0;
	for (auto& iter : this->code)
	{
		if (iter.cur == quad)
		{
			return index;
		}
		index++;
	}
	index = NOT_FOUND_INDEX;
	return index;
}


// 回填
void InterCode::backpatch(vector<int>& bpList,int quad)
{
	for (auto& iter : bpList)
	{
		int index = findByAddr(iter);
		code[index].res = to_string(quad);
	}
}

// 输出四元式
void InterCode::emit(Quadruple temp)
{
	code.push_back(temp);
	nextquad++;
}

// 说明语句翻译
void InterCode::defineConst()
{
	// TODO
	// const a = 1;
	// oprand = {a,1}
	// 调用对应的符号表接口
	// 
	// 防止重定义
}

// 定义语句翻译
void InterCode::defineVariable()
{
	// int a = 1;
	// operand = {a,1}
	// 防止重定义
}

// 赋值语句翻译
void InterCode::assignStatement()
{
	// operand
	// TODO

	// 生成四元式
	
	// 这里要根据表达式的类别
	// a = b
	// (=,b,-,a)
	// a = b + c
	// (=,b,c,a)

}

// M 翻译
void InterCode::MStatement()
{
	M* tmp = new M(nextquad);
	eleStack.push_back((VN*)tmp);
	//TFStack.push_back(vector<int>(1, cur_addr));
}

// N 翻译
void InterCode::NStatement()
{
	// TFStack.push_back(vector<int>(1, cur_addr));

	// Quadruple emit(cur_addr++, "j", "-", "-", "-");
	// code.push_back(emit);

	N* tmp = new N();
	VN* vp = (VN*)tmp;
	eleStack.push_back((VN*)tmp);

	// 压栈，地址在nextlist回填
	emit(Quadruple(nextquad, "j", "-", "-", ""));
	nextquad++;

}


// 布尔表达式翻译总控程序
void InterCode::boolExpr()
{
	
}

void InterCode::andStatement()
{
	// E1 && M E2
	// eleStack 栈顶
	// E1 M E2
	int end = eleStack.size();
	auto E2 = eleStack[end - 1];
	auto M = eleStack[end - 2];
	auto E1 = eleStack[end - 3];

	// 此处待修改，归约结果应当从语法分析中获取
	// TODO
	auto reduction = new E("xx表达式","");
	

	// PPT P72
	// 回填
	backpatch(E1->getTrueList(), M->getNextquad());

	//  E.truelist = E2.truelist
	reduction->truelist = E2->getTrueList();

	// E.falselist = merge(E1.falselist,E2.falselist)
	merge(E1->getFalseList(), E2->getFalseList(), reduction->falselist);

	// 把 E1 M E2 pop出去
	for (int i = 0; i < 3; i++)
	{
		eleStack.pop_back();
	}
	delete E2;
	delete M;
	delete E1;

	eleStack.push_back((VN*)reduction);
}


void InterCode::orStatement()
{
	// E1 or M E2
	int end = eleStack.size();
	auto E2 = eleStack[end - 1];
	auto M = eleStack[end - 2];
	auto E1 = eleStack[end - 3];

	// PPT 71页
	E* reduction = new E("xx表达式","");
	
	backpatch(E1->getFalseList(), M->getNextquad());

	merge(E1->getTrueList(), E2->getTrueList(), reduction->truelist);

	reduction->falselist = E2->getFalseList();

	// 消除E1 M E2
	for (int i = 0; i < 3; i++)
	{
		eleStack.pop_back();
	}
	delete E2;
	delete M;
	delete E1;

	eleStack.push_back((VN*)reduction);
}


void InterCode::notStatement()
{
	// E -> not E1
	int end = eleStack.size();
	auto E1 = eleStack[end - 1];

	//PPT 73
	E* reduction = new E("xx表达式", "");

	reduction->truelist = E1->getFalseList();

	reduction->falselist = E1->getTrueList();

	eleStack.pop_back();

	delete E1;

	eleStack.push_back((VN*)reduction);
}

// 依赖符号表的具体实现
// 实现基本类似关系表达式
void InterCode::relopStatement()
{
	
}

//if E then S1
void InterCode::if_then_statement()
{
	// PPT 84
	// S -> if E then M S1
	int end = eleStack.size();
	auto S1 = eleStack[end - 1];
	auto M = eleStack[end - 2];
	auto E = eleStack[end - 3];

	S* reduction = new S();

	backpatch(E->getTrueList(), M->getNextquad());
	merge(E->getFalseList(), S1->getNextList(),reduction->getNextList());

	for (int i = 0; i < 3; i++)
	{
		eleStack.pop_back();
	}
	delete S1;
	delete M;
	delete E;

	eleStack.push_back((VN*)reduction);
}

// if E then M1 S1 N else M2 S2
void InterCode::if_else_statement()
{
	// PPT 84
	int end = eleStack.size();
	auto S2 = eleStack[end - 1];
	auto M2 = eleStack[end - 2];
	auto N  = eleStack[end - 3];
	auto S1 = eleStack[end - 4];
	auto M1 = eleStack[end - 5];
	auto E  = eleStack[end - 6];

	
	S* reduction = new S();
	
	backpatch(E->getTrueList(), M1->getNextquad());
	backpatch(E->getFalseList(), M2->getNextquad());

	vector<vector<int> > list;
	list.push_back(S1->getNextList());
	list.push_back(N->getNextList());
	list.push_back(S2->getNextList());

	merge(list, reduction->getNextList());

	for (int i = 0; i < 6; i++)
	{
		eleStack.pop_back();
	}
	delete S2;
	delete M2;
	delete N;
	delete S1;
	delete S1;
	delete M1;
	delete E;

	eleStack.push_back((VN*)reduction);
}

// while do
void InterCode::while_do_statement()
{
	int end = eleStack.size();
	auto S1 = eleStack[end - 1];
	auto M2 = eleStack[end - 2];
	auto  E = eleStack[end - 3];
	auto M1 = eleStack[end - 4];

	S* reduction = new S();
	
	backpatch(S1->getNextList(), M1->getNextquad());
	backpatch(E->getTrueList(), M2->getNextquad());

	reduction->nextlist = E->getFalseList();
	
	stringstream ss;
	ss << M1->getNextquad();
	emit(Quadruple(nextquad,"j", "-", "-",ss.str()));
	nextquad++;
}




void InterCode::genCode(int reduceType,vector<E> operand)
{
	switch (reduceType)
	{
		case DEFINE_CONST:
			defineConst();
			break;
		case DEFINE_VARIABLE:
			defineVariable();
			break;
		case ASSIGNMENT:
			assignStatement();
		case BOOL_EXPR:
			boolExpr();
		default:
			return;
	}
}