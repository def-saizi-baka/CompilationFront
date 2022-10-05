#include <iostream>
#include <stack>
#include <fstream>
#include "FA.h"

using namespace std;


/******************************


********************************/
map<char, int> FA::pri_op = {
	{'(',0},
	{')',1},
	{'|',2},
	{'.',3},
	{'*',4},
};


VNode::VNode(int idx)
{
	this->idx = idx;
	this->isEnd = false;
}


/******************************
function : build NFA by or operation
paramter : keepEnd -> 保留or连接前的结束状态
********************************/
void VNode::setEdge(char ch, int idx)
{
	arcs[ch].push_back(idx);
}

/******************************
function : build NFA by or operation
paramter : keepEnd -> 保留or连接前的结束状态
********************************/
void FA::mergeFAbyOr(FA& fa,bool keepEnd)
{
	if (!keepEnd) {
		this->mgraph[this->endNode].isEnd = false;
		fa.mgraph[fa.endNode].isEnd = false;
	}

	int cnt = this->mgraph.size();
	
	// 增加起始/终止结点
	this->mgraph.push_back(VNode(cnt));
	this->mgraph.push_back(VNode(cnt + 1));

	// 增加新起始点 -> 旧起始点
	// 修改起点
	this->mgraph[cnt].setEdge(Epsilon_CH, this->begNode);
	this->begNode = cnt;

	// 增加旧终止点 -> 新终止点
	// 修改终点
	this->mgraph[this->endNode].setEdge(Epsilon_CH, cnt + 1);
	this->endNode = cnt + 1;

	
	// 增加fa的边
	cnt = cnt + 2;
	// 修改fa边的索引
	for (int i = 0; i < fa.mgraph.size(); i++)
	{
		VNode& vt = fa.mgraph[i];
		vt.idx += cnt;
		for (auto iter = vt.arcs.begin(); iter != vt.arcs.end(); iter++)
		{
			for (auto& vj : iter->second)
				vj += cnt;
		}
	}

	// 连接fa
	this->mgraph.insert(this->mgraph.end(),
						fa.mgraph.begin(),
						fa.mgraph.end());

	fa.begNode += cnt;
	fa.endNode += cnt;

	// 增加新起始点 -> 旧起始点
	this->mgraph[this->begNode].setEdge(Epsilon_CH, fa.begNode);

	// 增加旧终止点 -> 新终止点
	this->mgraph[fa.endNode].setEdge(Epsilon_CH, this->endNode);
	this->mgraph[this->endNode].isEnd = true;
}


/******************************
function : build NFA by closure operation

********************************/
void FA::mergeFAbyClosure()
{
	// 关闭终点
	this->mgraph[this->endNode].isEnd = false;

	// 增加新起点和新终点
	int cnt = this->mgraph.size();
	this->mgraph.push_back(VNode(cnt));
	this->mgraph.push_back(VNode(cnt + 1));
	
	// 增加旧终点到旧起点的连线
	this->mgraph[this->endNode].setEdge(Epsilon_CH,this->begNode);

	// 增加新起点到新终点的连线
	this->mgraph[cnt].setEdge(Epsilon_CH, cnt + 1);

	// 增加新起点到旧起点
	this->mgraph[cnt].setEdge(Epsilon_CH,this->begNode);

	// 增加旧终点到新终点
	this->mgraph[this->endNode].setEdge(Epsilon_CH, cnt + 1);


	// 修改起点，终点
	this->begNode = cnt;
	this->endNode = cnt + 1;
	this->mgraph[this->endNode].isEnd = true;
}

/******************************
function : build NFA by link operation

********************************/
void FA::mergeFAbyLink(FA& fa)
{
	// 关闭end状态
	fa.mgraph[fa.endNode].isEnd = false;

	// 修改fa边的索引
	int cnt = this->mgraph.size();
	for (int i = 0; i < fa.mgraph.size(); i++)
	{
		VNode& vt = fa.mgraph[i];
		vt.idx += cnt;
		for (auto iter = vt.arcs.begin(); iter != vt.arcs.end(); iter++)
		{
			for (auto& vj : iter->second)
				vj += cnt;
		}
	}

	// 连接fa
	this->mgraph.insert(this->mgraph.end(),
						fa.mgraph.begin(),
						fa.mgraph.end());
	// 连接边
	this->mgraph[this->endNode].setEdge(Epsilon_CH, fa.begNode + cnt);

	// 修改新终点
	this->endNode = fa.endNode + cnt;
	this->mgraph[this->endNode].isEnd = true;
}


/*******************************
function: merge NFA by specific operation
********************************/
void FA::mergerFA(stack<FA>& tmp, char op)
{
	FA fa1 = tmp.top();
	tmp.pop();
	if (op == '|') {
		FA fa2 = tmp.top();
		tmp.pop();
		fa2.mergeFAbyOr(fa1);
		tmp.push(fa2);
	}
	else if (op == '.') {
		FA fa2 = tmp.top();
		tmp.pop();
		fa2.mergeFAbyLink(fa1);
		tmp.push(fa2);
	}
	else {
		fa1.mergeFAbyClosure();
		tmp.push(fa1);
	}
}

/*******************************
function: print NFA
********************************/
void FA::showFA()
{
	cout << "begNode\t:\t" << this->begNode << endl;
	cout << "endNode\t:\t" << this->endNode << endl;
	for (int i = 0; i < this->mgraph.size(); i++)
	{
		cout << this->mgraph[i].idx;
		if (this->mgraph[i].isEnd)
			cout << "(end)\t:\t";
		else
			cout << "\t:\t";
		for (auto iter = this->mgraph[i].arcs.begin(); iter != this->mgraph[i].arcs.end(); iter++)
		{
			for (auto& vj : iter->second)
			{
				cout << "(" << iter->first << ")->" << vj << ",\t";
			}
		}
		cout << endl;
	}

	/*for (int i = 0; i < this->mgraph.size(); i++)
	{
		if (this->mgraph[i].isEnd)
			cout << i << endl;
	}*/
}

/*******************************
function: 部分字母表符号与正规表达式的符号重复
		  因此单独构造NFA，并连接
********************************/

void FA::initOP()
{
	string ops[] = { "*","(",")","|",".","||" };
	FA fa(ops[0],READ_STRING);
	
	this->begNode = fa.begNode;
	this->endNode = fa.endNode;
	this->mgraph = fa.mgraph;
	this->mgraph[this->endNode].isEnd = true;

	int len = 6;
	for (int i = 1; i < len; i++)
	{
		FA tmp(ops[i],READ_STRING);
		this->mergeFAbyOr(tmp, KEEP_END);
	}
	//this->showFA();
}

/********************************************
function：parse the regnex string   
delta	：[0-9][a-z][A-Z][;_:+-*]
*********************************************/
void FA::readRegex(string& reg)
{
	stack<FA> fas;
	stack<char> op;

	for (auto p = reg.begin(); p!=reg.end(); p++) {
		// '(' 直接压栈，不判断优先级
		if (*p == '('){
			op.push(*p);
		}
		else if (*p == '|' || *p == '*' || *p == '.' || *p == ')') {
			if (op.empty()){
				op.push(*p);
			}
			else {
				char top = op.top();
				// 优先级低于操作栈顶元素 && 栈非空
				while (FA::pri_op[*p] <= FA::pri_op[top]) {
					op.pop();
					mergerFA(fas, top);
					if (!op.empty()) {
						top = op.top();
					}
					else {
						break;
					}
				}
				if (*p == ')') {
					op.pop();
				}
				else {
					op.push(*p);
				}
			}
		}
		else {
			fas.push(FA(*p));
		}
	}

	while (!op.empty())
	{
		char ch = op.top();
		op.pop();

		mergerFA(fas, ch);
	}

	FA& fa_tmp = fas.top();

	this->begNode = fa_tmp.begNode;
	this->endNode = fa_tmp.endNode;
	this->mgraph = fa_tmp.mgraph;
	this->mgraph[this->endNode].isEnd = true;
}

/********************************************
function：parse the regnex file(one mode,one line)
delta	：[0-9][a-z][A-Z][;_:+-]
*********************************************/
void FA::readFile(string& filename)
{
	int sym_idx = 0;
	ifstream fin(filename, ios::in);
	if (!fin.is_open()) {
		cerr << "No config file!" << endl;
	}
	
	// 首先增加'*','|','||','(',')','.'，这些字符与正规表达式重复符号
	initOP();

	string reg;
	while (getline(fin, reg)) {
		FA tmp = FA(reg,READ_REGNEX);
		this->mergeFAbyOr(tmp,KEEP_END);
		//this->showFA();
	}

}

/********************************************
function：no regnex,just normol string
delta	：{ "*","(",")","|",".","||" }
*********************************************/
void FA::readStr(string& s)
{
	FA fa(char(*s.begin()));
	for (auto p = s.begin() + 1; p != s.end(); p++)
	{
		FA tmp(*p);
		fa.mergeFAbyLink(tmp);
	}
	this->begNode = fa.begNode;
	this->endNode = fa.endNode;
	this->mgraph = fa.mgraph;
	this->mgraph[this->endNode].isEnd = true;
}

FA::FA(char ch)
{
	mgraph.push_back(VNode(0));
	mgraph.push_back(VNode(1));

	mgraph[0].setEdge(ch, 1);

	this->begNode = 0;
	this->endNode = 1;
	mgraph[endNode].isEnd = true;
}

FA::FA(string& s, int type)
{
	if (type == READ_REGNEX) {
		this->readRegex(s);
	}
	else if (type == READ_STRING) {
		this->readStr(s);
	}
	else if (type == READ_FILE_BY_LINE) {
		this->readFile(s);
	}
}

DFA::DFA(string reg) : FA(reg)
{
	
}

bool DFA::checkStr(string& in,int& sym_idx,int& err_t)
{
	int cur = this->begNode;
	for (auto p = in.begin(); p != in.end(); p++)
	{
		// 转移（判断是否存在转移）
		try {
			int next_idx = this->mgraph[cur].arcs[*p][0];
			// 完成转移
			cur = next_idx;
		}
		catch (exception e) {
			// 找不到对应转移时
			err_t = NO_Trans;
			return false;
		}
		// 判断是否需要转移
	}
	return true;
}