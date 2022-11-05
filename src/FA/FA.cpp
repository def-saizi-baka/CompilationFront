#include <iostream>
#include <stack>
#include <fstream>
#include "FA.h"

using namespace std;


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
	for (int i = 0; i < fa.mgraph.size(); i++)
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
	for (int i = 0; i < fa.mgraph.size(); i++)
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
	for (int i = 0; i < this->mgraph.size(); i++)
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

/*******************************
function: 部分字母表符号与正规表达式的符号重复
		  因此单独构造NFA，并连接
********************************/

void FA::initOP()
{
	string ops[] = { "*","(",")","|",".","||", "*=", "|=" };
    int endTypes[] = {-1,-2-3,-4,-5,-6, -7, -8};
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


bool FA::checkStr(const string& in,int& sym_idx,int& err_t){
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
                end_state = getFirstPriState(this->mgraph[cur].endState);   
                index++;                         
            }
            res.push_back(pair<string, int>{nowBuffer, end_state});
            cur = this->begNode;
            nowBuffer = "";
        }
        else{
            index++;                                            // 字符串指针指向下一个字符
        }
    }

    for(auto ss:res){
        cout << ss.first << " " << ss.second << endl;
    }
	return false;
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