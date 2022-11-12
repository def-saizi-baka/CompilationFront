#include "NFA.h"
using namespace std;

int main(){
    NFA nfa;
    fstream fp("testNFAtoDFA2.txt", ios::in);
    string line;
    // 设置起始节点与终止节点
    getline(fp, line);
    int src_node = atoi(line.c_str());
    getline(fp, line);
    int dst_node = atoi(line.c_str());
    // 读取文件, 新增节点与转移函数
    while(getline(fp, line)){
        // 分析读入的数据
        int src_id, dst_id;
        char trans_ch;
        get_argv(src_id, dst_id, trans_ch, line);

        // 新建节点 / 边
        nfa.addSrc(src_id, dst_id, trans_ch);
    }

    nfa.sortById();
    nfa.setInitNode(src_node);
    nfa.setEndNode(dst_node);
    
    NFA dfa = nfa.toDFA();
    
    dfa.sortById();
    dfa.toString();
    return 0;
}