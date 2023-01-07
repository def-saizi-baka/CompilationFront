#include <iostream>
#include <stack>
#include <fstream>
#include "../include/FA.h"
#include <stdlib.h>
#include <string.h>
#include "../include/config.h"
#include "../include/Exception.h"

using namespace std;

extern config con;

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
function: ������ĸ�������������ʽ�ķ����ظ�
		  ��˵�������NFA��������
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
		// ������ʽ
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
    symbolTable.insert(ch);         // ����Զ������ű�

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
    symbolTable.insert(Epsilon_CH); // ���ű��ʼ��
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
    if(end_set.empty()) return -1;
    else return *(end_set.begin());
}

/*
	�������һ�����ֽ�ɴ���token
*/
vector<token> FA::checkStr(const string& in,int& sym_idx,int& err_t,int line){
	int cur = this->begNode;
    vector<pair<string, int>> res;
    string nowBuffer;
    int end_state = -1;
    unsigned int index = 0;
	//int last_M_state = NONE_ENDSTATE; // M����

    while(index <= in.length()){
        // �ж��Ƿ����ת�ƺ���
        if(index < in.length() &&this->mgraph[cur].arcs.count(in[index])){
            vector<int> vist_end;
            while(this->mgraph[cur].arcs.count(in[index])){
                cur = *(this->mgraph[cur].arcs[in[index]].begin()); // ת�Ƶ���һ���ڵ�
                nowBuffer += in[index];                             // ��ȡ��ǰ��    
                vist_end.push_back(getFirstPriState(this->mgraph[cur].endState));
                index++;                         
            }
			end_state = vist_end.back();
            // �ع�, �ҵ���һ����Ϊ-1���ִ�
            if(end_state < 0){
                int find_first_true = vist_end.size()-1;
                // ��ѯvisit����
                while(find_first_true>=0){
                    if(vist_end[find_first_true]>=0){
                        break;
                    }
                    find_first_true--;
                }
                // �ع�index
                if(find_first_true>=0){
                    // �޸��ַ�������
                    index -= (vist_end.size()-find_first_true-1);    
                    // �޸�end_state
                    end_state = vist_end[find_first_true];  
                    // �޸�  nowBuffer
                    string tmp;
                    for(int i=0; i<=find_first_true; i++){
                        tmp += nowBuffer[i];
                    }
                    nowBuffer = tmp;
                }
                else{
                    con.log("[ERROR] �ʷ��������ִ���, �������ĳ���Ϊ: " + nowBuffer);
					throw lexException(string("token ") + nowBuffer + string(" is not allowed here!"), line);
                    return vector<token>{token{line,"wrong",-1}};
                }
            }
			// �ڶ��� { ǰ��¼��һ�ε�ֵ
			if (end_state == IF_ENDSTATE) {
				last_M_state = IF_ENDSTATE;
			}
			else if (end_state == ELSE_END_STATE) {
				// else ��Ҫ��ǰ����һ�� N 
				res.push_back(pair<string, int>{N_String, con.get_symbols()[N_String]});
				last_M_state = ELSE_END_STATE;
			}

			// �ڶ��� "{" ʱ������һ�ζ�������if/while/else������M/N
			if (end_state == LEFT_BLOCK_STATE) {
				switch (last_M_state){
					// IF E then M S
					case IF_ENDSTATE: {
						res.push_back(pair<string, int>{M_String, con.get_symbols()[M_String]});
						break;
					}
					
					// IF E then M1 S1 N else M2 S2
					case ELSE_END_STATE: {
						res.push_back(pair<string, int>{M_String, con.get_symbols()[M_String]});
						break;
					}
					case WHILE_ENDSTATE: {
						res.push_back(pair<string, int>{M_String, con.get_symbols()[M_String]});
						break;
					}
					default:
						break;
				}
				// �޸�last_M_stateΪһ��״̬
				last_M_state = NONE_ENDSTATE;
			}

            res.push_back(pair<string, int>{nowBuffer, end_state});
			// �ڶ��� && �� || �Ĳ���
			if (end_state == AND_ENDSTATE || end_state == OR_ENDSTATE) {
				res.push_back(pair<string, int>{M_String, con.get_symbols()[M_String]});
			}

            cur = this->begNode;
            nowBuffer = "";

			if (end_state == WHILE_ENDSTATE) {
				// while �ĵ�һ�� MҪ����ѭ������ǰ
				last_M_state = IF_ENDSTATE;
				res.push_back(pair<string, int>{M_String, con.get_symbols()[M_String]});
			}
        }
        else{
            index++;                                            // �ַ���ָ��ָ����һ���ַ�
        }
    }

    vector<token> rres;
    for(auto ss:res){
        //cout << ss.first << " " << ss.second << endl;
        rres.push_back({line, ss.first, ss.second});
    }
	return rres;
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

	// ����endState
	int endStateSize = endState.size();
	memcpy(cur, (char*)&endStateSize, sizeof(endStateSize));
	cur += sizeof(endStateSize);
	for (auto iter : endState) {
		memcpy(cur, (char*)&iter, sizeof(iter));
		cur += sizeof(iter);
	}


	// ����arcs
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