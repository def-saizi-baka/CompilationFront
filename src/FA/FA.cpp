#include <iostream>
#include <stack>
#include <fstream>
#include "FA.h"

using namespace std;


// ��ӡ����
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
paramter : keepEnd -> ����or����ǰ�Ľ���״̬
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
paramter : keepEnd -> ����or����ǰ�Ľ���״̬
********************************/
void FA::mergeFAbyOr(FA& fa,bool keepEnd)
{
	if (!keepEnd) {
		this->mgraph[this->endNode].isEnd = false;
		fa.mgraph[fa.endNode].isEnd = false;
	}

	int cnt = this->mgraph.size();
	
	// ������ʼ/��ֹ���
	this->mgraph.push_back(VNode(cnt));
	this->mgraph.push_back(VNode(cnt + 1));

	// ��������ʼ�� -> ����ʼ��
	// �޸����
	this->mgraph[cnt].setEdge(Epsilon_CH, this->begNode);
	this->begNode = cnt;

	// ���Ӿ���ֹ�� -> ����ֹ��
	// �޸��յ�
	this->mgraph[this->endNode].setEdge(Epsilon_CH, cnt + 1);
	this->endNode = cnt + 1;

	
	// ����fa�ı�
	cnt = cnt + 2;
	// �޸�fa�ߵ�����
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

	// ����fa
	this->mgraph.insert(this->mgraph.end(),
						fa.mgraph.begin(),
						fa.mgraph.end());

	fa.begNode += cnt;
	fa.endNode += cnt;

	// ��������ʼ�� -> ����ʼ��
	this->mgraph[this->begNode].setEdge(Epsilon_CH, fa.begNode);

	// ���Ӿ���ֹ�� -> ����ֹ��
	this->mgraph[fa.endNode].setEdge(Epsilon_CH, this->endNode);
	this->mgraph[this->endNode].isEnd = true;

    // �ϲ���ĸ��
    symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());
}


/******************************
function : build NFA by closure operation

********************************/
void FA::mergeFAbyClosure()
{
	// �ر��յ�
	this->mgraph[this->endNode].isEnd = false;

	// �������������յ�
	int cnt = this->mgraph.size();
	this->mgraph.push_back(VNode(cnt));
	this->mgraph.push_back(VNode(cnt + 1));
	
	// ���Ӿ��յ㵽����������
	this->mgraph[this->endNode].setEdge(Epsilon_CH,this->begNode);

	// ��������㵽���յ������
	this->mgraph[cnt].setEdge(Epsilon_CH, cnt + 1);

	// ��������㵽�����
	this->mgraph[cnt].setEdge(Epsilon_CH,this->begNode);

	// ���Ӿ��յ㵽���յ�
	this->mgraph[this->endNode].setEdge(Epsilon_CH, cnt + 1);


	// �޸���㣬�յ�
	this->begNode = cnt;
	this->endNode = cnt + 1;
	this->mgraph[this->endNode].isEnd = true;
}

/******************************
function : build NFA by link operation

********************************/
void FA::mergeFAbyLink(FA& fa)
{
	// �ر�end״̬
	fa.mgraph[fa.endNode].isEnd = false;

	// �޸�fa�ߵ�����
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

	// ����fa
	this->mgraph.insert(this->mgraph.end(),
						fa.mgraph.begin(),
						fa.mgraph.end());
	// ���ӱ�
	this->mgraph[this->endNode].setEdge(Epsilon_CH, fa.begNode + cnt);

	// �޸����յ�
	this->endNode = fa.endNode + cnt;
	this->mgraph[this->endNode].isEnd = true;

    // �ϲ���ĸ��
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
function: ������ĸ�������������ʽ�ķ����ظ�
		  ��˵�������NFA��������
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
function��parse the regnex string   
delta	��[0-9][a-z][A-Z][;_:+-*]
*********************************************/
void FA::readRegex(string& reg, int endType)
{
	stack<FA> fas;
	stack<char> op;
	for (auto p = reg.begin(); p!=reg.end(); p++) {
		// '(' ֱ��ѹջ�����ж����ȼ�
		if (*p == '('){
			op.push(*p);
		}
		else if (*p == '|' || *p == '*' || *p == '.' || *p == ')') {
			if (op.empty()){
				op.push(*p);
			}
			else {
				char top = op.top();
				// ���ȼ����ڲ���ջ��Ԫ�� && ջ�ǿ�
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
    this->mgraph[this->endNode].endState.insert(endType);       // �����ֹ״̬���
    this->symbolTable.insert(fa_tmp.symbolTable.begin(), fa_tmp.symbolTable.end());
}

/********************************************
function��parse the regnex file(one mode,one line)
delta	��[0-9][a-z][A-Z][;_:+-]
*********************************************/
void FA::readFile(string& filename)
{
	int sym_idx = 0;
	ifstream fin(filename, ios::in);
	if (!fin.is_open()) {
		cerr << "No config file!" << endl;
	}
	
	// ��������'*','|','||','(',')','.'����Щ�ַ���������ʽ�ظ�����
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
function��no regnex,just normol string
delta	��{ "*","(",")","|",".","||" }
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
    this->mgraph[endNode].endState.insert(endType); // ��ӽ���״̬��Ӧ��id
    this->symbolTable.insert(fa.symbolTable.begin(), fa.symbolTable.end());
}

FA::FA(char ch)
{
	mgraph.push_back(VNode(0));
	mgraph.push_back(VNode(1));

	mgraph[0].setEdge(ch, 1);
    symbolTable.insert(ch);         // ����Զ������ű�

	this->begNode = 0;
	this->endNode = 1;
	mgraph[endNode].isEnd = true;
}

FA::FA(string& s, int type, int endType){
    symbolTable.insert(Epsilon_CH); // ���ű��ʼ��
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

/* �ݹ���ҵ�ǰ�ڵ㾭��Epsilon_CHƯ�Ƶ������нڵ� */
void FA::_findEpsilonSet(set<int>& find_res, int node_index){
    // ��鵱ǰ�ڵ��Ƿ��Ѿ���������
    if(find(find_res.begin(), find_res.end(), node_index)!=find_res.end())
        return;
    
    // û�б�����, push��find_res��
    find_res.insert(node_index);

    // ���ҵ�ǰ�ڵ��Ư��״̬
    set<int> next_epsilon_set = mgraph[node_index].arcs[Epsilon_CH];
    for(auto index : next_epsilon_set){
        _findEpsilonSet(find_res, index);
    }
}


/* ��ǰ�ڵ�����ch�����ص��Ӽ� */
set<int> FA::findNextNode(int src_index, char trans_ch){
    // ����ch�ɴﵽ���Ӽ�
    set<int> next_set_ch = mgraph[src_index].arcs[trans_ch];

    // �ɵ����Ӽ�epsilonƯ�Ƶ��Ӽ�
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
        // �ж��Ƿ����ת�ƺ���
        if(index < in.length() &&this->mgraph[cur].arcs.count(in[index])){
            while(this->mgraph[cur].arcs.count(in[index])){
                cur = *(this->mgraph[cur].arcs[in[index]].begin()); // ת�Ƶ���һ���ڵ�
                nowBuffer += in[index];                             // ��ȡ��ǰ��    
                end_state = getFirstPriState(this->mgraph[cur].endState);   
                index++;                         
            }
            res.push_back(pair<string, int>{nowBuffer, end_state});
            cur = this->begNode;
            nowBuffer = "";
        }
        else{
            index++;                                            // �ַ���ָ��ָ����һ���ַ�
        }
    }

    for(auto ss:res){
        cout << ss.first << " " << ss.second << endl;
    }
	return false;
}


FA FA::toDFA(){
    // �Ѿ��ϲ��Ľڵ�
    vector<set<int>> visit_set;
    // ��������
    queue<set<int>> work_set;
    // ����DFA�Ľڵ������ �ڵ㼯�� -> DFA�е�id
    map<set<int>, int> dfa_table;
    // ������DFA����
    FA dfa;

    // �����ʼ�ڵ�
    set<int> init_set = findNextNode(begNode, Epsilon_CH);
    init_set.insert(begNode);
    work_set.push(init_set);
    dfa_table.insert({init_set, dfa_table.size()}); // ���뵽�������
    dfa.begNode = 0;                                // ���ó�ʼ״̬

    // �Ӽ����취(�е���BFS)
    while(!work_set.empty()){
        // ��ȡ��������ǰ���еĵ�һ���ڵ�
        set<int> now_set = work_set.front();
        work_set.pop();

        // ��ֹ�ظ��ж�
        if(find(visit_set.begin(), visit_set.end(), now_set)!=visit_set.end()) continue;

        // ����Ϊ���ʹ�
        visit_set.push_back(now_set);

        // ��ǰ���ϴ���Ľڵ�id
        int src_index = dfa_table[now_set];    
        // cout << "src " << "id=" << src_index <<": ";
        // printSet(now_set);
        // cout << "dst: " << endl;

        // dfa����һ���ڵ�
        dfa.mgraph.push_back(VNode(src_index));


        // ������ĸ��ȥ���첻ͬ�ַ�ת�����Ӽ�
        for(auto ch : symbolTable){             
            // ����������ת�Ʒ���
            if(ch == Epsilon_CH) 
                continue;  

            // now_set ����c���� next_set           
            set<int> next_set;
            for(auto node_index : now_set){       // ���ҵ�ǰ�ڵ㼯���ҽڵ�id
                set<int> next_set_any = findNextNode(node_index, ch);
                next_set.insert(next_set_any.begin(), next_set_any.end());
            }
            
            // cout << ch << ": ";
            // printSet(next_set);

            // �жϷǿ�
            if(next_set.empty()) continue;

            // ���ҵ�ǰ�Ӽ��Ƿ��Ѿ������۹���
            bool is_visit = false;
            auto iter = find(visit_set.begin(), visit_set.end(), next_set);
            if(iter != visit_set.end()){
                // ��dfa�����һ����
                dfa.mgraph[src_index].setEdge(ch, dfa_table[*iter]);
                continue;
            }

            // ��ӵ���������
            work_set.push(next_set);
            dfa_table.insert({next_set, dfa_table.size()}); // ���뵽�������

            // ��һ�����ϴ���Ľڵ�id
            dfa.mgraph[src_index].setEdge(ch, dfa_table[next_set]);
        }
    }

    // ���ó�ʼ״̬�����״̬
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