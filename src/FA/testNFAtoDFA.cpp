#include "NFA.h"
using namespace std;

int main(){
    NFA nfa;
    fstream fp("testNFAtoDFA2.txt", ios::in);
    string line;
    // ������ʼ�ڵ�����ֹ�ڵ�
    getline(fp, line);
    int src_node = atoi(line.c_str());
    getline(fp, line);
    int dst_node = atoi(line.c_str());
    // ��ȡ�ļ�, �����ڵ���ת�ƺ���
    while(getline(fp, line)){
        // �������������
        int src_id, dst_id;
        char trans_ch;
        get_argv(src_id, dst_id, trans_ch, line);

        // �½��ڵ� / ��
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