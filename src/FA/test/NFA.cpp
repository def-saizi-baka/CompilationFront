#include "NFA.h"

// ��ӡ����
void printSet(set<int> s){
    cout << "{";
    for(auto ss : s){
        cout << ss << ' ';
    }
    cout << "}" << endl;
}


/**
 * @brief �ֽ��ַ���
 * 
 * @param src  ��ʼ�ڵ�id
 * @param dst   ��ֹ�ڵ�id
 * @param transfer_ch   ת���ַ�
 * @param input �����ַ���
 */
void get_argv(int& src, int& dst, char &transfer_ch,string input){
    stringstream ss;
    input.replace(input.find("->"), 2, " ");
    ss << input;
    ss >> src >> dst >> transfer_ch;
}


// ����һ���ڵ�, ͬʱ����һ��ת�ƺ���
VNode::VNode(int _id, int _dst, char _ch){
    id = _id;
    nextNode.push_back(Vsrc(_dst, _ch));
}

// ���һ����
void VNode::addNext(int _dst, char _ch){
    nextNode.push_back(Vsrc(_dst, _ch));
}

// ��� / ��ӡNFA�ַ���
void VNode::toString(){
    cout << this->id << ": "<<endl;
    for(auto node : nextNode){
        cout << this->id <<"->"<<node.getId()<<" transfer: "<<node.getCh()<<endl;
    }
}

// ������һ������(ֻ����һ��ת��)
set<int> VNode::nextSet(char trans_ch){
    set<int> dst;
    for(auto next_node : nextNode){
        if(next_node.getCh() == trans_ch){
            dst.insert(next_node.getId());
        }
    }
    return dst;
}

/*       NFA����        */

/**
 * @brief ������ʼ�ڵ� 
 * 
 * @param id �ڵ�id
 * @return �ҵ��ڵ㷵��0, �ڵ㲻���ڷ���-1
 */
int NFA::setInitNode(int id){
    for(uint32_t i=0; i<nodeArray.size(); i++){
        if(nodeArray[i].getId() == id){
            nodeArray[i].is_begin = true;
            return 0;
        }
    }
    return -1;
}

/**
 * @brief ������ֹ�ڵ� 
 * 
 * @param id �ڵ�id
 * @return �ҵ��ڵ㷵��0, �ڵ㲻���ڷ���-1
 */
int NFA::setEndNode(int id){
    for(uint32_t i=0; i<nodeArray.size(); i++){
        if(nodeArray[i].getId() == id){
            nodeArray[i].is_end = true;
            return 0;
        }
    }
    return -1;
}

// ��ȡ��ʼ�ڵ� / ����
int NFA::getInitNodeId(){
    for(auto node : nodeArray){
        if(node.is_begin){
            return node.getId();
        }
    }
    return -1;
}

// ��ȡ��ֹ�ڵ� / ����
set<int> NFA::getEndNodeId(){
    set<int> res;
    for(auto node : nodeArray){
        if(node.is_end){
            res.insert(node.getId());
        }
    }
    return res;
}

/**
 * @brief ���ҽڵ��б�
 * 
 * @param id 
 * @return �ڵ�����, �Ҳ�������-1
 */
int NFA::findNode(int id){
    for(int i=0; i<nodeArray.size(); i++){
        if(nodeArray[i].getId() == id){
            return i;
        }
    }
    return -1;
}

/**
 * @brief ���һ��ת�ƺ���, ���Զ��½��ڵ�
 * 
 * @param src_id ��ʼ�ڵ�
 * @param dst_id ��ֹ�ڵ�
 * @param transfer_ch ת���ַ�
 */
int NFA::addSrc(int src_id, int dst_id, char transfer_ch){
    // ���ҽ��
    int src_index = findNode(src_id);
    int dst_index = findNode(dst_id);
    // û�ҵ����½��ڵ�
    if(dst_index < 0){
        nodeArray.push_back(VNode(dst_id));
    }
    if(src_index < 0){
        nodeArray.push_back(VNode(src_id, dst_id, transfer_ch));
    }
    else{
        nodeArray[src_index].addNext(dst_id, transfer_ch);
    }

    // ��ӷ��ű�
    if(find(symbolTable.begin(), symbolTable.end(), transfer_ch)==symbolTable.end()){
        symbolTable.push_back(transfer_ch);
    }
    return 0;
}


/**
 * @brief ͨ���ڵ�ID��������, ��֤NFA�ڵ������е�������ڵ�idһ��
 */
int NFA::sortById(){
    sort(nodeArray.begin(), nodeArray.end());
    return 0;
}

/**
 * @brief ��������ڵ���Ϣ
 */
void NFA::toString(){
    cout << "{ ";
    for(auto ch : symbolTable){
        cout << ch <<' ';
    }
    cout << "}" << endl;

    for(auto node : nodeArray){
        if(node.is_begin) cout << "(Init) ";
        if(node.is_end) cout << "(end)";
        node.toString();
    }
}

/**
 * @brief ��ǰ�ڵ�����ch���ܵ�����Ӽ�
 * 
 * @param node_id �ڵ�id
 * @param ch    ת���ַ�
 * @return �ܵ���ļ���
 */
set<int> NFA::findNextSet(int node_id, char ch){
    nextAny.clear();    // ��յݹ��õĽ���Ӽ�
    // ��ǰ�ڵ�����ch���ܵ���Ľڵ㼯��
    set<int> next_set_ch = nodeArray[node_id].nextSet(ch);
    // ��ǰ����ch��Ľڵ㼯����������ת�Ƶ���Ľڵ�
    for(auto next_node_id : next_set_ch){
        _findNextSet(next_node_id);
    }
    return nextAny;
}

void NFA::_findNextSet(int node_id){
    // �Ȳ��ҵ�ǰ�ڵ��Ƿ��Ѿ�������
    for(auto id : nextAny){
        if(id == node_id) return;
    }
    // û�б��ѹ�, ֱ��push��ȥ
    nextAny.insert(node_id);
    // ���ҵ�ǰ�ڵ��������ת��
    set<int> now_next_any = nodeArray[node_id].nextSet(S3S);
    // ���ŵݹ����²�ѯ��ȥ
    for(auto id : now_next_any){
        _findNextSet(id);
    }
}

/**
 * @brief ��ǰ�Ѿ��ź���ڵ��NFAת����DFA
 */
NFA NFA::toDFA(){
    // �Ѿ��ϲ��Ľڵ�
    vector<set<int>> visit_set;
    // ��������
    queue<set<int>> work_set;
    // ����DFA�Ľڵ������
    map<set<int>, int> dfa_table;
    // ������DFA����
    NFA dfa;
    
    // �����ʼ�ڵ㼯��
    set<int> init_set;
    for(auto node : nodeArray){
        if(node.is_begin){
            init_set.insert(node.getId());
            // Ѱ����״̬�ڵ�
            set<int> tmp = findNextSet(node.getId(), S3S);
            init_set.insert(tmp.begin(), tmp.end());
            break;
        }
    }
    work_set.push(init_set);
    dfa_table.insert({init_set, dfa_table.size()}); // ���뵽�������

    while(!work_set.empty()){
        // ��ȡ��������ǰ���еĵ�һ���ڵ�
        set<int> now_set = work_set.front();
        work_set.pop();

        // ��ֹ�ظ��ж�
        bool is_vit = false;
        for(auto vit : visit_set){
            if(vit == now_set){
                is_vit = true;
                break;
            }
        }
        if(is_vit) continue;

        // ����Ϊ���ʹ�
        visit_set.push_back(now_set);
        printSet(now_set);
        
        int src_id = dfa_table[now_set];        // ��ǰ���ϴ���Ľڵ�id

        // �����ַ���ȥת��
        for(auto ch : symbolTable){             // ת������
            if(ch == S3S) continue;             // ����������ת�Ʒ���
            set<int> next_set;
            for(auto node_id : now_set){       // ���ҵ�ǰ�ڵ㼯���ҽڵ�id
                set<int> next_set_any = findNextSet(node_id, ch);
                next_set.insert(next_set_any.begin(), next_set_any.end());
            }
            
            cout << ch << ": ";
            printSet(next_set);

            // �жϷǿ�
            if(next_set.empty()) continue;

            // �ж�û�б��ҵ�
            bool is_visit = false;
            for(auto tmp_set : visit_set){
                if(tmp_set == next_set){
                    is_visit = true;
                    dfa.addSrc(src_id, dfa_table[next_set], ch);
                    break;
                }
            }
            if(is_visit) continue;

            // ��ӵ���������
            work_set.push(next_set);
            dfa_table.insert({next_set, dfa_table.size()}); // ���뵽�������

            // ��һ�����ϴ���Ľڵ�id
            int dst_id = dfa_table[next_set];
            dfa.addSrc(src_id, dst_id, ch);

        }
    }

    // ������ʼ�ڵ�����ֹ�ڵ�
    dfa.setInitNode(0);
    set<int> nfa_end_node_set = getEndNodeId();
    for(auto dfa_node_set : dfa_table){
        for(auto id : dfa_node_set.first){
            // ����������ֹ�ڵ�
            if(find(nfa_end_node_set.begin(), nfa_end_node_set.end(), id) != nfa_end_node_set.end()){
                dfa.setEndNode(dfa_node_set.second);
                break;
            }
        }
    }

    return dfa;
}
    