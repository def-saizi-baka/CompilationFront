#include "../include/intercode.h"
#include <iostream>
#include <sstream>
#include "../include/symbolTable.h"
#include <fstream>

//extern config con;
// 此处宏定义没有用，单纯分工合作的时候，我先填上去
// 归约的种类应该从Parse类中读取静态变量
#define DEFINE_CONST 0
#define DEFINE_VARIABLE 1
#define ASSIGNMENT 2
#define BOOL_EXPR 3

#define NOT_FOUND_INDEX -1

symbolTable symTable;

// 四元式类
Quadruple::Quadruple(const int cur,const string op, const string left_num, const string right_num, const string res)
{
	this->cur = cur;
	this->op = op;
	this->left_num = left_num;
	this->right_num = right_num;
	this->res = res;
}

ostream& operator<<(ostream& out, const Quadruple& t)
{
	out << to_string(t.cur) << ":";
	out << "(" << t.op << "," << t.left_num << "," << t.right_num << "," << t.res << ")\n";
	return out; 
}

ofstream& operator<<(ofstream& out, const Quadruple& t)
{
	out << to_string(t.cur) << ":";
	out << "(" << t.op << "," << t.left_num << "," << t.right_num << "," << t.res << ")\n";
	return out; 
}

VN::VN()
{
	this->type = 0;
}

// 表达式元素
E::E(string _name):VN()
{
	this->name = _name;
}

E::E(string name, string val) :VN() {
	this->name = name;
	this->value = val;
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
	this->pst = nullptr;
}

InterCode::InterCode(parserTree& pst)
{
	this->nextquad = 100;
	this->pst = &pst;
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

void InterCode::operationStatement(const node& root) {
	/*
		<操作语句> → <二目操作>
		<操作语句> → <单目操作>
	*/
	const node* now_1 = root.kids[0];
	switch (root.kids[0]->symbol)
	{
		case 1029:	// 二目操作
		{
			/*
				<二目操作> → <赋值语句>
				<二目操作> → <计算语句>
			*/
			const node* now_2 = now_1->kids[0];	// 现在根节点为 now_2 (赋值语句1030)
			switch (now_1->kids[0]->symbol)
			{
				case 1030:	// <二目操作1029> → <赋值语句1030>
				{
					/*
					<赋值语句> → <变量名> <赋值符号> <函数调用>
					<赋值语句> → <变量名> <赋值符号> <表达式>
					*/
					string varName = now_2->kids[0]->value;	// 取变量名
					// 检查变量名是否未定义
					symbol* it = symTable.look(varName);
					if (it != nullptr) { //此处为已定义

					}
					else {//此处为未定义
						string msg = "symbol " + varName + " is undefined.";
						throw UndefinedDefinitionsException(msg, this->line);
					}

					// 判断是 <函数调用>还是<表达式>
					if (now_2->kids[2]->symbol == 1023) {
						// 表达式
						E* e = (E*)eleStack.back();
						eleStack.pop_back();
						// 生成一条赋值语句
						emit(Quadruple(nextquad, ":=", e->value, "_", varName));
						// 生成E压栈
						// TODO 
						E* e_added = new E("", symTable.get_temp());
						eleStack.push_back(e_added);
					}
					else if(now_2->kids[2]->symbol == 1017) {
						// 函数调用未实现
					}
					break;
				}
				case 1032:  // // <二目操作1029> → <计算语句>
				{
					// <计算语句> → <表达式> <计算符号> <表达式>
					E* e_right = (E*)eleStack.back();	// 右操作数
					eleStack.pop_back();
					E* e_left = (E*)eleStack.back();	// 左操作数
					eleStack.pop_back();
					// nextquad 操作符 左操作数 右操作数 临时变量
					string temp = symTable.get_temp();
					emit(Quadruple(nextquad, now_2->kids[1]->leaf[0]->value, e_left->value, e_right->value,temp));
					// 当前表达式压栈
					E* e_added = new E("", temp);
					eleStack.push_back(e_added);
					break;
				}					
				default:
					break;
				}
		}


		break;
		case 1037:	// 单目操作1037 now_1
			/*
				<操作语句> → <单目操作1037>
				<单目操作> → <单目操作符1038><变量名>
				<单目操作> → <变量名><单目操作符>
			*/
		{
			if (now_1->kids[0]->symbol == 1038) {
				// <单目操作> → <单目操作符1038><变量名>
				string varName = now_1->leaf[0]->value;
				string varOperator = now_1->leaf[1]->value;

				string temp = symTable.get_temp();
				emit(Quadruple(nextquad, varOperator, "", varName, temp));
				E* e_added = new E("", temp);
				eleStack.push_back(e_added);
			}
			else{
				// <单目操作> → <变量名><单目操作符>
				string varName = now_1->leaf[1]->value;
				string varOperator = now_1->leaf[0]->value;
				emit(Quadruple(nextquad, varOperator, varName, "", "T?"));
				E* e_added = new E("", "T?");
				eleStack.push_back(e_added);
			}
		}
		break;
	default:
		break;
	}
}

// 定义语句翻译 不支持非常量
void InterCode::defineVariable(const node& root)
{
	// 左孩子的叶节点是变量类型
	const node* defineTree = root.kids[0];
	string valType = defineTree->kids[0]->leaf[0]->value; //int

	if (defineTree->leaf.size() > 2) {
		// 右孩子是一个类似赋值语句的东西
		const node* assignmentTree = defineTree->kids[1]->kids[0];
		string valName = assignmentTree->kids[0]->value;

		if (symTable.look(valName) != NULL) {
			//cout << "重定义的变量: "<< valName << endl;
			string msg = "symbol " + valName + " is multiply defined.";
			throw MultipleDefinitionsException(msg, this->line);
			//exit(-1);
		}

		// 获取表达式的值
		E* e = (E*)eleStack.back();
		eleStack.pop_back();
		// 符号表新增定义
		symTable.enter(valName, string_type(valName), false);
		// 增加四元式
		this->emit(Quadruple{ nextquad,":=", e->value, "", valName });
	}
	else {
		string valName = defineTree->leaf[1]->value;
		if (symTable.look(valName) != NULL) {
			//cout << "重定义的变量: " << valName << endl;
			string msg = "symbol " + valName + " is multiply defined.";
			throw MultipleDefinitionsException(msg, this->line);
			//exit(-1);
		}
		// 符号表新增一个定义
		symTable.enter(valType, string_type(valName), false);
	}

	// int a = 1;
	// operand = {a,1}
	// 防止重定义
}

// 赋值语句翻译
void InterCode::assignStatement()
{
	// 无需特判该函数，已在表达式处理函数中实现赋值
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
	delete M1;
	delete E;
	// 这里是不是没弹出 while也是？

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

	for (int i = 0; i < 4; i++)
	{
		eleStack.pop_back();
	}

	delete M2;
	delete S1;
	delete M1;
	delete E;
}

// 计算表达式的
void InterCode::expression_statement(const node& root) {
	/* 分类讨论 */ 
	/*
		<表达式> → <操作语句>
		<表达式>→<标识符>
		<表达式>→<(> <表达式> <)>
	*/
	switch (root.kids[0]->symbol)
	{
		case 1026:	// <表达式> → <操作语句1026>
			operationStatement(*(root.kids[0]));
			break;
		case 1008:	// <表达式>→<标识符1008>
		{
			// TODO
			string valName = root.leaf[0]->value;

			vector<node*> kids;
			// 如果value 对应的是变量/常变量
			if (this->pst->get_kids(&root, con.get_symbols()["标识符"], kids)) {
				for(const auto& t :kids ){
					if(t->symbol == con.get_symbols()["变量名"]){
						if (symTable.look(valName) == nullptr) {
							string msg = "symbol " + valName + " is undefined.";
							throw UndefinedDefinitionsException(msg, this->line);
						}
						
						break;
					}
					else if(t->symbol == con.get_symbols()["常量"]){
						break;
					}
				}
			}


			E* e = new E("", valName);
			eleStack.push_back((VN*)e);
		}
			break;
		case 100:	// <表达式>→<(> <表达式> < )>
			break;
		default:
			break;
	}
}


void InterCode::genCode(const node& root,int line)
{
	int count = 0;
	this->line = line;
	switch (root.symbol)
	{
		case 1034: // 变量定义语句
			//cout << "1034" << endl;
			defineVariable(root);
			//this->outputCode();
			//cout << endl;
			break;
		case 1023: // 表达式
			//cout << "1023" << endl;
			expression_statement(root);
			//this->outputCode();
			//cout << endl;
			break;
		case DEFINE_CONST:
			defineConst();
			break;
		case ASSIGNMENT:
			assignStatement();
		case BOOL_EXPR:
			boolExpr();
		default:
			return;
	}
}

void InterCode::outputCode(const char* filename)
{
	cout << "********************************************" << endl;
	cout << "          InterCode                         " << endl;
	if (filename == NULL)
	{
		int i = code.size() - 1;
		for (uint32_t i = 0; i < code.size();i++)
		{
			cout << code[i] << endl;
		}
		code = vector<Quadruple>();
	}
	else 
	{
		// 输出到文件
		ofstream fout(filename, ios::out);
		// TODO
	}
	cout << "********************************************" << endl;
}