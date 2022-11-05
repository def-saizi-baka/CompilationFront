#include "NFA.h"

// 打印集合
void printSet(set<int> s){
    cout << "{";
    for(auto ss : s){
        cout << ss << ' ';
    }
    cout << "}" << endl;
}


/**
 * @brief 分解字符串
 * 
 * @param src  起始节点id
 * @param dst   终止节点id
 * @param transfer_ch   转移字符
 * @param input 输入字符串
 */
void get_argv(int& src, int& dst, char &transfer_ch,string input){
    stringstream ss;
    input.replace(input.find("->"), 2, " ");
    ss << input;
    ss >> src >> dst >> transfer_ch;
}


// 构造一个节点, 同时增加一条转移函数
VNode::VNode(int _id, int _dst, char _ch){
    id = _id;
    nextNode.push_back(Vsrc(_dst, _ch));
}

// 添加一条边
void VNode::addNext(int _dst, char _ch){
    nextNode.push_back(Vsrc(_dst, _ch));
}

// 输出 / 打印NFA字符串
void VNode::toString(){
    cout << this->id << ": "<<endl;
    for(auto node : nextNode){
        cout << this->id <<"->"<<node.getId()<<" transfer: "<<node.getCh()<<endl;
    }
}

// 返回下一个集合(只考虑一步转移)
set<int> VNode::nextSet(char trans_ch){
    set<int> dst;
    for(auto next_node : nextNode){
        if(next_node.getCh() == trans_ch){
            dst.insert(next_node.getId());
        }
    }
    return dst;
}

/*       NFA部分        */

/**
 * @brief 设置起始节点 
 * 
 * @param id 节点id
 * @return 找到节点返回0, 节点不存在返回-1
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
 * @brief 设置终止节点 
 * 
 * @param id 节点id
 * @return 找到节点返回0, 节点不存在返回-1
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

// 获取起始节点 / 索引
int NFA::getInitNodeId(){
    for(auto node : nodeArray){
        if(node.is_begin){
            return node.getId();
        }
    }
    return -1;
}

// 获取终止节点 / 索引
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
 * @brief 查找节点列表
 * 
 * @param id 
 * @return 节点索引, 找不到返回-1
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
 * @brief 添加一条转移函数, 并自动新建节点
 * 
 * @param src_id 起始节点
 * @param dst_id 终止节点
 * @param transfer_ch 转移字符
 */
int NFA::addSrc(int src_id, int dst_id, char transfer_ch){
    // 查找结点
    int src_index = findNode(src_id);
    int dst_index = findNode(dst_id);
    // 没找到就新建节点
    if(dst_index < 0){
        nodeArray.push_back(VNode(dst_id));
    }
    if(src_index < 0){
        nodeArray.push_back(VNode(src_id, dst_id, transfer_ch));
    }
    else{
        nodeArray[src_index].addNext(dst_id, transfer_ch);
    }

    // 添加符号表
    if(find(symbolTable.begin(), symbolTable.end(), transfer_ch)==symbolTable.end()){
        symbolTable.push_back(transfer_ch);
    }
    return 0;
}


/**
 * @brief 通过节点ID进行排序, 保证NFA节点数组中的索引与节点id一致
 */
int NFA::sortById(){
    sort(nodeArray.begin(), nodeArray.end());
    return 0;
}

/**
 * @brief 输出各个节点信息
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
 * @brief 当前节点输入ch后能到达的子集
 * 
 * @param node_id 节点id
 * @param ch    转移字符
 * @return 能到达的集合
 */
set<int> NFA::findNextSet(int node_id, char ch){
    nextAny.clear();    // 清空递归用的结果子集
    // 当前节点输入ch后能到达的节点集合
    set<int> next_set_ch = nodeArray[node_id].nextSet(ch);
    // 当前输入ch后的节点集合能无条件转移到达的节点
    for(auto next_node_id : next_set_ch){
        _findNextSet(next_node_id);
    }
    return nextAny;
}

void NFA::_findNextSet(int node_id){
    // 先查找当前节点是否已经搜索过
    for(auto id : nextAny){
        if(id == node_id) return;
    }
    // 没有被搜过, 直接push进去
    nextAny.insert(node_id);
    // 查找当前节点的无条件转移
    set<int> now_next_any = nodeArray[node_id].nextSet(S3S);
    // 接着递归向下查询下去
    for(auto id : now_next_any){
        _findNextSet(id);
    }
}

/**
 * @brief 当前已经排好序节点的NFA转换成DFA
 */
NFA NFA::toDFA(){
    // 已经合并的节点
    vector<set<int>> visit_set;
    // 工作队列
    queue<set<int>> work_set;
    // 构造DFA的节点表索引
    map<set<int>, int> dfa_table;
    // 构造存放DFA的类
    NFA dfa;
    
    // 构造初始节点集合
    set<int> init_set;
    for(auto node : nodeArray){
        if(node.is_begin){
            init_set.insert(node.getId());
            // 寻找无状态节点
            set<int> tmp = findNextSet(node.getId(), S3S);
            init_set.insert(tmp.begin(), tmp.end());
            break;
        }
    }
    work_set.push(init_set);
    dfa_table.insert({init_set, dfa_table.size()}); // 插入到结果集合

    while(!work_set.empty()){
        // 获取并弹出当前队列的第一个节点
        set<int> now_set = work_set.front();
        work_set.pop();

        // 防止重复判断
        bool is_vit = false;
        for(auto vit : visit_set){
            if(vit == now_set){
                is_vit = true;
                break;
            }
        }
        if(is_vit) continue;

        // 设置为访问过
        visit_set.push_back(now_set);
        printSet(now_set);
        
        int src_id = dfa_table[now_set];        // 当前集合代表的节点id

        // 根据字符表去转换
        for(auto ch : symbolTable){             // 转移条件
            if(ch == S3S) continue;             // 跳过无条件转移符号
            set<int> next_set;
            for(auto node_id : now_set){       // 查找当前节点集查找节点id
                set<int> next_set_any = findNextSet(node_id, ch);
                next_set.insert(next_set_any.begin(), next_set_any.end());
            }
            
            cout << ch << ": ";
            printSet(next_set);

            // 判断非空
            if(next_set.empty()) continue;

            // 判断没有被找到
            bool is_visit = false;
            for(auto tmp_set : visit_set){
                if(tmp_set == next_set){
                    is_visit = true;
                    dfa.addSrc(src_id, dfa_table[next_set], ch);
                    break;
                }
            }
            if(is_visit) continue;

            // 添加到工作队列
            work_set.push(next_set);
            dfa_table.insert({next_set, dfa_table.size()}); // 插入到结果集合

            // 下一个集合代表的节点id
            int dst_id = dfa_table[next_set];
            dfa.addSrc(src_id, dst_id, ch);

        }
    }

    // 设置起始节点与终止节点
    dfa.setInitNode(0);
    set<int> nfa_end_node_set = getEndNodeId();
    for(auto dfa_node_set : dfa_table){
        for(auto id : dfa_node_set.first){
            // 集合中有终止节点
            if(find(nfa_end_node_set.begin(), nfa_end_node_set.end(), id) != nfa_end_node_set.end()){
                dfa.setEndNode(dfa_node_set.second);
                break;
            }
        }
    }

    return dfa;
}
    