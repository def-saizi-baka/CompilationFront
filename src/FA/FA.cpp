#include <iostream>
#include <stack>
#include <fstream>
#include "FA.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

using namespace std;

extern config con;

// 打印集合
void printSet(set<int> s){
    cout << "{";
    for(auto ss : s){
        cout << ss << ' ';
    }
    cout << "}" << endl;
}


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
	arcs[ch].insert(idx);
}

void VNode::setEdge(char ch, set<int> idx_set){
    arcs[ch].insert(idx_set.begin(), idx_set.end());
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
	for (unsigned int i = 0; i < fa.mgraph.size(); i++)
	{
		VNode& vt = fa.mgraph[i];
		vt.idx += cnt;
		for (auto iter = vt.arcs.begin(); iter != vt.arcs.end(); iter++){
            set<int> tmp_set;
			for (auto vj : iter->second) 
                tmp_set.insert(vj+cnt);
            iter->second = tmp_set;
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

    // 合并字母表
    symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());
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
	for (unsigned int i = 0; i < fa.mgraph.size(); i++)
	{
		VNode& vt = fa.mgraph[i];
		vt.idx += cnt;
		for (auto iter = vt.arcs.begin(); iter != vt.arcs.end(); iter++){
            set<int> tmp_set;
			for (auto vj : iter->second)
				tmp_set.insert(vj+cnt);
            iter->second = tmp_set;
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

    // 合并字母表
    symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());
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
	for (unsigned int i = 0; i < this->mgraph.size(); i++)
	{
		cout << this->mgraph[i].idx;
		if (this->mgraph[i].isEnd){
			cout << "(end)\t:\t{";
            for(auto state : this->mgraph[i].endState){
                cout << state << ' ';
            }
            cout << "}\t";
        }
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

/*********************************************
function: 部分字母表符号与正规表达式的符号重复
		  因此单独构造NFA，并连接
**********************************************/

void FA::initOP()
{
	string ops[] = { "*","(",")","|",".","||", "*=", "|=" };
    int endTypes[] = {19, 100,102,31,8, 33, 39, 46};
	FA fa(ops[0],READ_STRING, endTypes[0]);
	
	this->begNode = fa.begNode;
	this->endNode = fa.endNode;
	this->mgraph = fa.mgraph;
	this->mgraph[this->endNode].isEnd = true;
    this->mgraph[this->endNode].endState.insert(endTypes[0]);
    this->symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());

	int len = 6;
	for (int i = 1; i < len; i++)
	{
		FA tmp(ops[i],READ_STRING, endTypes[i]);
		this->mergeFAbyOr(tmp, KEEP_END);
	}
	//this->showFA();
}

/********************************************
function：parse the regnex string   
delta	：[0-9][a-z][A-Z][;_:+-*]
*********************************************/
void FA::readRegex(string& reg, int endType)
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
			if(*p == '\\')
			{
				p++;
				if(*p=='s'){
					fas.push(FA(' '));
				}else{
					fas.push(FA(*p));
				}
			}else{
				fas.push(FA(*p));
			}
			
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
    this->mgraph[this->endNode].endState.insert(endType);       // 添加终止状态标记
    this->symbolTable.insert(fa_tmp.symbolTable.begin(), fa_tmp.symbolTable.end());
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
        stringstream ss;
        int end_type;
        ss << reg;
        ss >> reg >> end_type; 
		FA tmp = FA(reg,READ_REGNEX, end_type);
		this->mergeFAbyOr(tmp,KEEP_END);
		//this->showFA();
	}

}

/********************************************
function：no regnex,just normol string
delta	：{ "*","(",")","|",".","||" }
*********************************************/
void FA::readStr(string& s, int endType)
{
	FA fa(char(*s.begin()));
	for (auto p = s.begin() + 1; p != s.end(); p++){
		FA tmp(*p);
		fa.mergeFAbyLink(tmp);
	}
	this->begNode = fa.begNode;
	this->endNode = fa.endNode;
	this->mgraph = fa.mgraph;
	this->mgraph[this->endNode].isEnd = true;
    this->mgraph[endNode].endState.insert(endType); // 添加结束状态对应的id
    this->symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());
}


/*******************************
 function: get the tree
 delta   :   
 *****************************/
void FA::readSymolTable()
{
	int sym_idx = 0;
	bool first = false;
	vector<regex_exp> regexList = con.get_regex();
	for(auto& iter : regexList)
	{
		if(!first)
		{
			int flag = iter.raw ? READ_STRING :READ_REGNEX; 
			FA fa(iter.regex, flag, iter.endType);
			this->begNode = fa.begNode;
			this->endNode = fa.endNode;
			this->mgraph = fa.mgraph;
			this->mgraph[this->endNode].isEnd = true;
			this->mgraph[this->endNode].endState.insert(iter.endType);
			this->symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());
			
			first = true;
		}
		// 正规表达式
		if(iter.raw == 1)
		{
			FA tmp = FA(iter.regex,READ_STRING,iter.endType);
			this->mergeFAbyOr(tmp,KEEP_END);
		}else{
			FA tmp = FA(iter.regex,READ_REGNEX,iter.endType);
			this->mergeFAbyOr(tmp,KEEP_END);
		}
	}
}

FA::FA(char ch)
{
	mgraph.push_back(VNode(0));
	mgraph.push_back(VNode(1));

	mgraph[0].setEdge(ch, 1);
    symbolTable.insert(ch);         // 添加自动机符号表

	this->begNode = 0;
	this->endNode = 1;
	mgraph[endNode].isEnd = true;
}

FA::FA()
{
	begNode = 0;
	endNode = 0;
	symbolTable.insert(Epsilon_CH);
}

FA::FA(string& s, int type, int endType){
    symbolTable.insert(Epsilon_CH); // 符号表初始化
	if (type == READ_REGNEX) {
		this->readRegex(s, endType);
	}
	else if (type == READ_STRING) {
		this->readStr(s, endType);
	}
	else if (type == READ_FILE_BY_LINE) {
		this->readFile(s);
	}else if(type == READ_SYMBOLTABLE) {
		this->readSymolTable();
	}
}

/* 递归查找当前节点经过Epsilon_CH漂移到的所有节点 */
void FA::_findEpsilonSet(set<int>& find_res, int node_index){
    // 检查当前节点是否已经被搜索过
    if(find(find_res.begin(), find_res.end(), node_index)!=find_res.end())
        return;
    
    // 没有被搜索, push到find_res中
    find_res.insert(node_index);

    // 查找当前节点可漂移状态
    set<int> next_epsilon_set = mgraph[node_index].arcs[Epsilon_CH];
    for(auto index : next_epsilon_set){
        _findEpsilonSet(find_res, index);
    }
}


/* 当前节点输入ch所返回的子集 */
set<int> FA::findNextNode(int src_index, char trans_ch){
    // 输入ch可达到的子集
    set<int> next_set_ch = mgraph[src_index].arcs[trans_ch];

    // 可到达子集epsilon漂移的子集
    set<int> return_set;
    for(auto index : next_set_ch){
        _findEpsilonSet(return_set, index);
    }

    return return_set;
}

/**
 * @brief Get the First Pri State object
 * 
 * @param end_set end state set
 * @return int 
 */
int getFirstPriState(set<int> end_set){
    return *(end_set.begin());
}


vector<int> FA::checkStr(const string& in,int& sym_idx,int& err_t){
	int cur = this->begNode;
    vector<pair<string, int>> res;
    string nowBuffer;
    int end_state = -1;
    unsigned int index = 0;

    while(index <= in.length()){
        // 判断是否存在转移函数
        if(index < in.length() &&this->mgraph[cur].arcs.count(in[index])){
            while(this->mgraph[cur].arcs.count(in[index])){
                cur = *(this->mgraph[cur].arcs[in[index]].begin()); // 转移到下一个节点
                nowBuffer += in[index];                             // 读取当前串    
                index++;                         
            }
			end_state = getFirstPriState(this->mgraph[cur].endState);
            res.push_back(pair<string, int>{nowBuffer, end_state});
            cur = this->begNode;
            nowBuffer = "";
        }
        else{
            index++;                                            // 字符串指针指向下一个字符
        }
    }

    vector<int> rres;
    for(auto ss:res){
        cout << ss.first << " " << ss.second << endl;
        rres.push_back(ss.second);
    }
	return rres;
}


FA FA::toDFA(){
    // 已经合并的节点
    vector<set<int>> visit_set;
    // 工作队列
    queue<set<int>> work_set;
    // 构造DFA的节点表索引 节点集合 -> DFA中的id
    map<set<int>, int> dfa_table;
    // 构造存放DFA的类
    FA dfa;

    // 构造初始节点
    set<int> init_set = findNextNode(begNode, Epsilon_CH);
    init_set.insert(begNode);
    work_set.push(init_set);
    dfa_table.insert({init_set, dfa_table.size()}); // 插入到结果集合
    dfa.begNode = 0;                                // 设置初始状态

    // 子集构造法(有点像BFS)
    while(!work_set.empty()){
        // 获取并弹出当前队列的第一个节点
        set<int> now_set = work_set.front();
        work_set.pop();

        // 防止重复判断
        if(find(visit_set.begin(), visit_set.end(), now_set)!=visit_set.end()) continue;

        // 设置为访问过
        visit_set.push_back(now_set);

        // 当前集合代表的节点id
        int src_index = dfa_table[now_set];    
        // cout << "src " << "id=" << src_index <<": ";
        // printSet(now_set);
        // cout << "dst: " << endl;

        // dfa新增一个节点
        dfa.mgraph.push_back(VNode(src_index));


        // 根据字母表去构造不同字符转换的子集
        for(auto ch : symbolTable){             
            // 跳过无条件转移符号
            if(ch == Epsilon_CH) 
                continue;  

            // now_set 输入c到达 next_set           
            set<int> next_set;
            for(auto node_index : now_set){       // 查找当前节点集查找节点id
                set<int> next_set_any = findNextNode(node_index, ch);
                next_set.insert(next_set_any.begin(), next_set_any.end());
            }
            
            // cout << ch << ": ";
            // printSet(next_set);

            // 判断非空
            if(next_set.empty()) continue;

            // 查找当前子集是否已经被讨论过了
            bool is_visit = false;
            auto iter = find(visit_set.begin(), visit_set.end(), next_set);
            if(iter != visit_set.end()){
                // 在dfa中添加一条边
                dfa.mgraph[src_index].setEdge(ch, dfa_table[*iter]);
                continue;
            }

            // 添加到工作队列
            work_set.push(next_set);
            dfa_table.insert({next_set, dfa_table.size()}); // 插入到结果集合

            // 下一个集合代表的节点id
            dfa.mgraph[src_index].setEdge(ch, dfa_table[next_set]);
        }
    }

    // 设置初始状态与结束状态
    dfa.begNode = 0;
    for(auto sub_set : dfa_table){
        for(auto index : sub_set.first){
            if(mgraph[index].isEnd && (!mgraph[index].endState.empty())){
                dfa.mgraph[sub_set.second].isEnd = true;
                dfa.mgraph[sub_set.second].endState.insert(mgraph[index].endState.begin(),
                    mgraph[index].endState.end());
                // cout << "(end): "; 
                // break;
            }
        }
        // cout << sub_set.second << " ";
        // printSet(sub_set.first);
    }
    dfa.endNode = -1;


    return dfa;
}


void VNode::saveNode(ofstream& fout)
{
	int size = 1024 * 1024;
	char* buffer = new char[size];
	char* cur = buffer;
	if (buffer == NULL)
	{
		cerr << "Can't get enough memory!" << endl;
		exit(-1);
	}
	
	memcpy(cur, (char*)&idx, sizeof(idx));
	cur += sizeof(idx);
	memcpy(cur, (char*)&isEnd, sizeof(isEnd));
	cur += sizeof(isEnd);

	// 保存endState
	int endStateSize = endState.size();
	memcpy(cur, (char*)&endStateSize, sizeof(endStateSize));
	cur += sizeof(endStateSize);
	for (auto iter : endState) {
		memcpy(cur, (char*)&iter, sizeof(iter));
		cur += sizeof(iter);
	}


	// 保存arcs
	int mapSize = arcs.size();
	memcpy(cur, (char*)&mapSize, sizeof(mapSize));
	cur += sizeof(mapSize);
	for (auto iter = arcs.begin(); iter != arcs.end(); iter++)
	{
		memcpy(cur, (char*)&iter->first, sizeof(iter->first));
		cur += sizeof(iter->first);

		int nextNodeSize = iter->second.size();
		memcpy(cur, (char*)&nextNodeSize, sizeof(nextNodeSize));
		cur += sizeof(nextNodeSize);
		for (auto i : iter->second) {
			memcpy(cur, (char*)&i, sizeof(i));
			cur += sizeof(i);
		}
	}
	fout.write(buffer, cur - buffer);
	delete[] buffer;
}

void VNode::loadNode(ifstream& fin)
{
	char* buffer = new char[1024 * 1024];
	char* cur = buffer;
	fin.read((char*)&idx, sizeof(idx));
	fin.read((char*)&isEnd, sizeof(isEnd));
	

	// read endState
	int endStateSize;
	fin.read((char*)&endStateSize, sizeof(endStateSize));
	fin.read(buffer, endStateSize * sizeof(int));
	for (int i = 0; i < endStateSize; i++)
	{
		int tmp;
		memcpy((char*)&tmp, cur, sizeof(int));
		cur += sizeof(int);
		endState.insert(tmp);
	}

	//read map
	int mapSize = 0;
	fin.read((char*)&mapSize, sizeof(mapSize));
	for (int i = 0; i < mapSize; i++)
	{
		char key;
		set<int> value;
		fin.read(&key, sizeof(key));

		cur = buffer;
		int nextNodeSize;
		fin.read((char*)&nextNodeSize, sizeof(nextNodeSize));
		fin.read(buffer, nextNodeSize * sizeof(int));
		for (int i = 0; i < nextNodeSize; i++)
		{
			int tmp;
			memcpy((char*)&tmp, cur, sizeof(int));
			cur += sizeof(int);
			value.insert(tmp);
		}
		arcs[key] = value;
	}
	delete[] buffer;
}

VNode::VNode()
{
	idx = 0;
	isEnd = false;
}


/**********************************
function : save FA binary data 
**********************************/
void FA::saveDFA(const string& model_file)
{
	ofstream fout;
	fout.open(model_file, ios::out | ios::binary);
	if (!fout.is_open())
	{
		cerr << "Can't open the model file" << endl;
		exit(-1);
	}
	
	fout.write((char*)&begNode, sizeof(begNode));
	fout.write((char*)&endNode, sizeof(endNode));

	int mgraphSize = mgraph.size();
	fout.write((char*)&mgraphSize, sizeof(mgraphSize));
	for (auto node : mgraph)
	{
		node.saveNode(fout);
	}

	int tableLen = sizeof(tableLen);
	char* buffer = new char[1024 * 1024];
	for (auto iter : symbolTable)
	{
		memcpy(buffer + tableLen, (char*)&iter, sizeof(iter));
		tableLen += sizeof(iter);
	}
	memcpy(buffer, (char*)&tableLen, sizeof(tableLen));
	fout.write(buffer, tableLen);
	delete[] buffer;
}

/*********************************
function : load FA binary data
*********************************/
void FA::loadDFA(const string& model_file)
{
	ifstream fin;
	fin.open(model_file, ios::in | ios::binary);
	if (!fin.is_open())
	{
		cerr << "Can't open the model file" << endl;
		exit(-1);
	}

	char* buffer = new char[1024 * 1024];
	char* cur = buffer;
	
	fin.read((char*)&begNode, sizeof(begNode));
	fin.read((char*)&endNode, sizeof(endNode));

	int graphSize;
	fin.read((char*)&graphSize, sizeof(graphSize));
	for (int i = 0; i < graphSize; i++)
	{
		VNode tmp;
		tmp.loadNode(fin);
		mgraph.push_back(tmp);
	}

	int tableLen;
	fin.read((char*)&tableLen, sizeof(tableLen));
	fin.read(buffer, tableLen);
	for (int i = 0; i < tableLen; i++)
	{
		char tmp = *cur;
		symbolTable.insert(tmp);
		cur += sizeof(char);
	}

	/*int endNodeLen;
	fin.read((char*)&endNodeLen, sizeof(endNodeLen));
	fin.read(buffer, endNodeLen);
	for (int i = 0; i < endNodeLen; i += sizeof(int))
	{
		int tmp;
		memcpy((char*)&tmp,cur, sizeof(int));
		endNode.insert(tmp);
		cur += sizeof(int);
	}*/
	delete[] buffer;
}