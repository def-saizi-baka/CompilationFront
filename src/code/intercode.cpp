#include "../include/intercode.h"
#include <iostream>
#include <sstream>
#include "../include/symbolTable.h"
#include <fstream>

//extern config con;

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
	out << "(" << t.op << "," << t.left_num << "," << t.right_num << "," << t.res << ")";
	return out; 
}

ofstream& operator<<(ofstream& out, const Quadruple& t)
{
	out << to_string(t.cur) << ":";
	out << "(" << t.op << "," << t.left_num << "," << t.right_num << "," << t.res << ")";
	return out; 
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

S::S(int nextquad)
{
	this->nextquad = nextquad;
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

void InterCode::assignExpression(const node& root, bool bool_flag)
{
	/*
		<赋值语句> → <变量名> <赋值符号> <函数调用>
		<赋值语句> → <变量名> <赋值符号> <表达式>
	*/
	string varName = root.kids[0]->value;	// 取变量名

	// 检查变量名是否未定义
	symbol* it = symTable.look(varName);
	if (it == nullptr) {//此处为未定义
		string msg = "symbol " + varName + " is undefined.";
		throw UndefinedDefinitionsException(msg, this->line);
	}

	// 常变量被赋值
	if (it->get_const()) {//改变为常变量
		string msg = "const symbol " + varName + " cannot be assigned";
		throw ConstantModificationException(msg, this->line);
	}

	// 判断是<函数调用>还是<表达式>
	if (root.kids[2]->symbol == 1023) {
		// 表达式
		E* e = (E*)eleStack.back();
		eleStack.pop_back();
		// 生成一条赋值语句
		emit(Quadruple(nextquad, root.kids[1]->leaf[0]->value, e->value, "_", varName));
		// 生成E压栈
		// TODO 
		E* e_added = new E("", symTable.get_temp());
		eleStack.push_back(e_added);
	}
	else if (root.kids[2]->symbol == 1017) {
		// 函数调用未实现
	}
}

void InterCode::calExpression(const node& root,bool bool_flag)
{
	// <计算语句> → <表达式> <计算符号> <表达式>
	E* e_right = (E*)eleStack.back();	// 右操作数
	eleStack.pop_back();
	E* e_left = (E*)eleStack.back();	// 左操作数
	eleStack.pop_back();
	// nextquad 操作符 左操作数 右操作数 临时变量
	string temp = symTable.get_temp();
	emit(Quadruple(nextquad, root.kids[1]->leaf[0]->value, e_left->value, e_right->value, temp));
	// 当前表达式压栈
	E* e_added = new E("", temp);
	eleStack.push_back(e_added);
}

void InterCode::logicExpression(string logicOp, bool bool_flag)
{
	//string symbol = root.kids[1]->leaf[0]->value;
	if (logicOp == "&&") {
		andExpression();
	}
	else if (logicOp == "||") {
		orExpression();
	}
	else if (logicOp == "!") {
		notExpression();
	}
}

void InterCode::selfChangeExpression(const node& root,bool is_front)
{
	string varName;
	string varOperator;
	if (is_front)
	{
		// <单目操作> → <单目操作符1038><变量名>
		varName = root.leaf[1]->value;
		varOperator = root.leaf[0]->value;
	}else
	{
		varName = root.leaf[0]->value;
		varOperator = root.leaf[1]->value;
	}

	// 未定义
	symbol* it = symTable.look(varName);
	if (it == nullptr) {//此处为未定义
		string msg = "symbol " + varName + " is undefined.";
		throw UndefinedDefinitionsException(msg, this->line);
	}

	// 常变量被赋值
	if (it->get_const()) {//改变为常变量
		string msg = "const symbol " + varName + " cannot be assigned";
		throw ConstantModificationException(msg, this->line);
	}
	string temp = symTable.get_temp();
	emit(Quadruple(nextquad, varOperator, "_", varName, temp));
	if (is_front)				// 前自增立刻修改值
	{
		emit(Quadruple(nextquad, ":=", temp,"_", varName));
	}
	E* e_added = new E("", temp);
	eleStack.push_back(e_added);
}

void InterCode::operationExpression(const node& root) {
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
				<二目操作> → <关系语句>
				<二目操作> → <逻辑语句>
			*/
			const node* now_2 = now_1->kids[0];	
			// 下面switch的根节点为 now_2 (赋值语句1030/计算语句1032/关系语句1047/逻辑语句1048)
			switch (now_1->kids[0]->symbol)
			{
				case 1030:	// <二目操作1029> → <赋值语句1030>
				{
					assignExpression(*now_2);
					break;
				}
				case 1032:  // <二目操作1029> → <计算语句1032>
				{
					calExpression(*now_2);
					break;
				}					
				case 1047:	
					// <二目操作> → <关系语句1047>
					// <关系语句> → <表达式> <关系符号> <表达式>
					relopExpression(*now_2);
					break;

				case 1048:	
					// <二目操作> → <逻辑语句1048>
					// <逻辑语句> → <表达式> <逻辑符号> <表达式>
					logicExpression(now_2->kids[1]->leaf[0]->value);
					break;

				default:
					break;
				}
		}
		break;
		case 1037:	// 单目操作1037 now_1
			/*
				<操作语句> → <单目操作1037>
				<单目操作> → <单目操作符1038><变量名> (这种情况要考虑not)
				<单目操作> → <变量名><单目操作符>
			*/
		{
			if (now_1->leaf[0]->symbol == con.get_symbols()["++"]
				|| now_1->leaf[0]->symbol == con.get_symbols()["--"])
			{
				selfChangeExpression(*now_1, true);
			}
			// not 运算
			else if (now_1->kids[0]->value == "!") {
				notExpression("!");
			}
			else if (now_1->leaf[1]->symbol == con.get_symbols()["++"]
					|| now_1->leaf[1]->symbol == con.get_symbols()["--"])
			{
				selfChangeExpression(*now_1, false);
			}
		}
		break;
	default:
		break;
	}
}

// 常变量定义语句翻译
void InterCode::defineConst(const node& root)
{
	vector<node*> leaf = root.leaf;
	string const_str = leaf[0]->value;
	string valType = leaf[1]->value;
	for (uint32_t i = 2; i < leaf.size();) {
		// 获取变量名
		string valName = leaf[i]->value;
		if (symTable.look(valName) != NULL) {
			// cout << "重定义的变量: "<< valName << endl;
			string msg = "symbol " + valName + " is multiply defined.";
			throw MultipleDefinitionsException(msg, this->line);
			//exit(-1);
		}
		// 操作符 = 
		string op_symbol = leaf[i+1]->value;
		// 获取值;
		// 获取表达式的值
		E* e = (E*)eleStack.back();
		eleStack.pop_back();
		// 符号表新增定义
		symTable.enter(valName, string_type(valType), true);
		// 增加四元式
		this->emit(Quadruple{ nextquad,":=", e->value, "_", valName });
		// 跳过 = 后面部分
		while (i < leaf.size() && leaf[i]->value != ",") {
			if (leaf[i++]->value == ",") {
				break;
			}
		}
		// 跳过逗号
		if (i < leaf.size() && leaf[i]->value == ",") {
			i++;
		}
			
	}

	// 转化成S
	S* s_tmp = new S(nextquad);
	eleStack.push_back(s_tmp);
}

// 定义语句翻译 不支持非常量
void InterCode::defineVariable(const node& root)
{
	// 左孩子的叶节点是变量类型
	const node* defineTree = root.kids[0];
	string valType = defineTree->kids[0]->leaf[0]->value; //int
	vector<string> name_arr;
	vector<string> value_arr;
		// 右孩子是一个类似赋值语句的东西
		vector<node*> leaf = defineTree->leaf;
		for (uint32_t i = 1; i < leaf.size();) {
			// 获取变量名
			string valName = leaf[i]->value;
			if (symTable.look(valName) != NULL) {
				//cout << "重定义的变量: "<< valName << endl;
				string msg = "symbol " + valName + " is multiply defined.";
				throw MultipleDefinitionsException(msg, this->line);
				//exit(-1);
			}
			if (i+1 >= leaf.size()||leaf[i + 1]->value == ",") { // int b,
				i+=2;
				// 符号表新增一个定义
				symTable.enter(valName, string_type(valType), false);
			}
			else if(leaf[i + 1]->value == "=") {	// int a=0, b
				name_arr.push_back(valName);
				// 获取表达式的值
				E* e = (E*)eleStack.back();
				eleStack.pop_back();
				value_arr.push_back(e->value);
				
				while (i < leaf.size() && leaf[i]->value != ",") {
					if (leaf[i++]->value == ",") {
						break;
					}
				}
				// 跳过逗号
				if (i < leaf.size() && leaf[i]->value == ",") {
					i++;
				}
			}
			else {
				cout << "Debug: 定义式错误" << endl;
				exit(-1);
			}
		}
		for (auto name : name_arr) {
			string val = value_arr.back();
			value_arr.pop_back();
			// 符号表新增定义
			symTable.enter(name, string_type(valType), false);
			// 增加四元式
			this->emit(Quadruple{ nextquad,":=", val, "_", name });
		}
	//else {
	//	string valName = defineTree->leaf[1]->value;
	//	if (symTable.look(valName) != NULL) {
	//		//cout << "重定义的变量: " << valName << endl;
	//		string msg = "symbol " + valName + " is multiply defined.";
	//		throw MultipleDefinitionsException(msg, this->line);
	//		//exit(-1);
	//	}
	//	// 符号表新增一个定义
	//	symTable.enter(valName, string_type(valType), false);
	//}

	// 转化成S
	S* s_tmp = new S(nextquad);
	eleStack.push_back(s_tmp);

}

// 表达式语句 -> 表达式
void InterCode::expression2statment() 
{
	// 获取E
	E* e = (E*)eleStack.back();
	eleStack.pop_back();
	// 转化成S
	S* s_tmp = new S(nextquad);
	eleStack.push_back(s_tmp);
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
	tmp->nextlist.push_back(nextquad);
	VN* vp = (VN*)tmp;
	eleStack.push_back((VN*)tmp);

	// 压栈，地址在nextlist回填
	emit(Quadruple(nextquad, "j", "-", "-", ""));

}

// 变量名做表达式的翻译
void InterCode::valNameExpression(const node& root,bool bool_flag)
{
	// TODO
	string valName = root.leaf[0]->value;

	vector<node*> kids;
	// 如果value 对应的是变量/常变量
	if (this->pst->get_kids(&root, con.get_symbols()["标识符"], kids)) {
		for (const auto& t : kids) {
			if (t->symbol == con.get_symbols()["变量名"]) {
				if (symTable.look(valName) == nullptr) {
					string msg = "symbol " + valName + " is undefined.";
					throw UndefinedDefinitionsException(msg, this->line);
				}
				break;
			}
			else if (t->symbol == con.get_symbols()["常量"]) {
				break;
			}
		}
	}
	E* e = new E("", valName);
	if (bool_flag) {
		e->truelist.push_back(nextquad);
		e->falselist.push_back(nextquad + 1);
		emit(Quadruple(nextquad, "jnz", valName, "_", "0"));
		emit(Quadruple(nextquad, "j", "_", "_", "0"));
	}
	eleStack.push_back((VN*)e);
}

// && 翻译
void InterCode::andExpression(bool bool_flag)
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
		auto reduction = new E("xx表达式", "");

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

// || 翻译
void InterCode::orExpression(bool bool_flag)
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

// !翻译
void InterCode::notExpression(bool bool_flag)
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
void InterCode::relopExpression(const node& root, bool bool_flag)
{
	string eValor = symTable.get_temp();
	E* e_tmp = new E("", eValor);
	e_tmp->truelist.push_back(nextquad);
	e_tmp->falselist.push_back(nextquad + 1);

	E* e_right = (E*)eleStack.back();	// 右操作数
	eleStack.pop_back();
	E* e_left = (E*)eleStack.back();	// 左操作数
	eleStack.pop_back();
	string relopOperator = root.kids[1]->leaf[0]->value;

	string jmp_op = "j"+ relopOperator;

	emit(Quadruple(nextquad, jmp_op, e_left->value, e_right->value,"0"));
	emit(Quadruple(nextquad, "j", "_", "_", "0"));

	eleStack.push_back(e_tmp);
	delete e_right;
	delete e_left;
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

	S* reduction = new S(nextquad);

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

	
	S* reduction = new S(nextquad);
	
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

	eleStack.push_back((VN*)reduction);
}

// while do
// while M1 E do M2 S1 
void InterCode::while_do_statement()
{
	int end = eleStack.size();
	auto S1 = eleStack[end - 1];
	auto M2 = eleStack[end - 2];
	auto  E = eleStack[end - 3];
	auto M1 = eleStack[end - 4];

	S* reduction = new S(nextquad + 1);
	
	backpatch(S1->getNextList(), M1->getNextquad());
	backpatch(E->getTrueList(), M2->getNextquad());

	reduction->nextlist = E->getFalseList();
	
	stringstream ss;
	ss << M1->getNextquad();
	emit(Quadruple(nextquad,"j", "_", "_",ss.str()));

	for (int i = 0; i < 4; i++)
	{
		eleStack.pop_back();
	}

	delete M2;
	delete S1;
	delete M1;
	delete E;

	eleStack.push_back((VN*)(reduction));
}

// 表达式
void InterCode::allExpression(const node& root, bool bool_flag)
{
	/*
		<表达式> → <操作语句>
		<表达式>→<标识符>
		<表达式>→<(> <表达式> <)>
		<表达式>→
	*/
	switch (root.kids[0]->symbol)
	{
		case 1026:	// <表达式> → <操作语句1026>
			operationExpression(*(root.kids[0]));
			break;
		case 1008:	// <表达式>→<标识符1008>
			valNameExpression(root, bool_flag);
			break;
		case 100:	// <表达式>→<(> <表达式> < )>
			break;
		default:
			break;
	}
}


// 语句块List
void InterCode::statemmentList(const node& root)
{
	if (root.kids[0]->symbol == con.get_symbols()["语句List"])
	{
		int end = eleStack.size();
		auto S1 = eleStack[end - 1];
		auto L = eleStack[end - 2];
		backpatch(L->getNextList(), L->getNextquad());

		S* reduction = new S(S1->getNextquad());

		reduction->nextlist = S1->getNextList();

		eleStack.pop_back();
		eleStack.pop_back();

		delete L;
		delete S1;

		eleStack.push_back((VN*)(reduction));
	}
	else if (root.kids[0]->symbol == con.get_symbols()["语句"])
	{
		int end = eleStack.size();
		auto S1 = eleStack[end - 1];
		
		S* reduction = new S(S1->getNextquad());
		reduction->nextlist = S1->getNextList();

		eleStack.pop_back();
		delete S1;
		
		eleStack.push_back((VN*)reduction);
	}
}

// 语句块生成
void InterCode::statementBlock(const node& root)
{
	
	if (root.kids.size() == 2) //< 语句块 > → < { > < } >
	{
		S* S1 = new S(nextquad);
		eleStack.push_back((VN*)S1);
	}
	else  // <语句块> → <{><语句List><}>
	{
		// 不需要做额外操作
	}
}


void InterCode::genCode(const node& root,int line)
{
	this->line = line;
	switch (root.symbol)
	{
		case 1034: // 变量定义语句
			defineVariable(root);
			break;
		case 1042:	// 常变量定义语句
			defineConst(root);
			break;
		case 1023: // 表达式翻译(包含赋值、计算、关系、逻辑运算)
			allExpression(root);
			break;
		case 1051:	// 布尔表达式
			allExpression(root, true);
			break;
		case 1019:	// 表达式语句
			expression2statment();
		case 1016:	// 语句
			break;
		case 1015:	// 语句List
			statemmentList(root);
			break;
		case 1014:	// 语句块:
			statementBlock(root);
			break;
		case 1020:	// 选择语句
			if (root.kids.size() > 6)
				if_else_statement();
			else
				if_then_statement();
			break;
		case 1021:  // 循环语句
			while_do_statement();
			break;
		case 1049:	// M
			MStatement();
			break;
		case 1050:	// N
			NStatement();
			break;
		default:
			return;
	}

	 /*this->outputCode();
	 cout << endl;*/
}


void InterCode::outputCode(const char* filename)
{
	if (filename == NULL)
	{
		cout << "********************************************" << endl;
		cout << "          InterCode                         " << endl;
		int i = code.size() - 1;
		for (uint32_t i = 0; i < code.size();i++)
		{
			cout << code[i] << endl;
		}
		//code = vector<Quadruple>();
		cout << "********************************************" << endl;
	}
	else 
	{
		// 输出到文件
		ofstream fout(filename, ios::out);
		// TODO
		fout << "                 InterCode                  " << endl;
		fout << "********************************************" << endl;
		int i = code.size() - 1;
		for (uint32_t i = 0; i < code.size(); i++)
		{
			fout << code[i] << endl;
		}
		fout << "********************************************" << endl;
		fout.close();
	}
}