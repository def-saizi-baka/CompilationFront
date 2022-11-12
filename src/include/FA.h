#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include <sstream>
using namespace std;


#define Epsilon_CH		'\0'
#define NO_Trans		1
#define READ_FILE_BY_LINE	1
#define READ_REGNEX			2
#define READ_STRING			3
#define KEEP_END		true
#define READ_SYMBOLTABLE     4


class FA;
class DFA;

class VNode {
    private:
        int idx;
        bool isEnd;
        set<int> endState;
        map<char, set<int>> arcs;
        friend FA;
        friend DFA;
    public:
        VNode(int idx);
        VNode();
        void setEdge(char ch, int idx);
        void setEdge(char ch, set<int> idx_set);
        void saveNode(ofstream& fout);
        void loadNode(ifstream& fin);
};

class FA{
    protected:
        int begNode;
        int endNode;
        vector<VNode> mgraph;       // 节点表
        set<char>  symbolTable;    // 字母表

        static map<char, int> pri_op;
        void initOP();
        void mergerFA(stack<FA>& tmp, char op);
        FA(char ch);

        void readRegex(string& regex, int endType);
        void readStr(string& s, int endType);
        void readFile(string& filename);
        void readSymolTable();
        void _findEpsilonSet(set<int>& find_res, int node_index);

    public:
        FA();
        FA(string& s,int type = READ_REGNEX, int endType = -1);
        
        void mergeFAbyOr(FA& tmp, bool keepEnd = false);
        void mergeFAbyLink(FA& tmp);
        void mergeFAbyClosure();

        void showFA();
        
        // 查找索引为src_id节点输入trans_ch后转移能到的节点集合
        set<int> findNextNode(int src_id, char trans_ch);

        FA toDFA();
        // 检查串是否能被接收
        vector<int> checkStr(const string& in,int& sym_idx,int& err_t);

        // 保存FA
        void saveDFA(const string& model_file);

        // 加载FA
        void loadDFA(const string& model_file);

};

class DFA: public FA{
    protected:
        set<int> endNode;
    public:
        DFA(string reg, int type = READ_REGNEX);
        bool checkStr(string& in,int& sym_idx,int& err_t);
};

