#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<queue>
#include<set>
#include<map>

using namespace std;

// 用#代替yiboxilong
#define S3S '#'

// 读取文件的函数
void get_argv(int& src, int& dst, char &transfer_ch,string input);

/* 转移函数(边) */
class Vsrc{
    private:
        int idx;    // 转移节点id
        char ch;    // 转移字符
    public:
        // 构造函数
        Vsrc(int id, int _ch){ idx = id; ch = _ch;}
        // 获取
        int getId(){return this->idx;}
        char getCh(){return this->ch;}
};


/* NFA中的每一个节点 */
class VNode{
    private:
        int id;
        vector<Vsrc> nextNode;
    public:
        bool is_begin=false;
        bool is_end=false;

        // 构造函数
        VNode(int _id, int _dst, char _ch);
        VNode(int _id){ id = _id; }

        // 获取节点id
        int getId(){return this->id;};

        // 添加一条边
        void addNext(int _dst, char _ch);

        // 输出 / 打印整个节点值, 转移函数
        void toString();

        //  重写比较函数
        bool operator < (VNode &b){ return this->id < b.getId(); }

        // 返回下一个集合(只考虑一步转移)
        set<int> nextSet(char trans_ch);
};

/* NFA类实现 */
class NFA{
    private:
        vector<VNode> nodeArray;    // 节点列表
        vector<char> symbolTable;   // 符号表
        set<int> nextAny;           // 对于无条件转移的递归判断
    public:

        // 设置起始节点
        int setInitNode(int id);

        // 设置终止节点
        int setEndNode(int id);

        // 获取起始节点 / 索引
        int getInitNodeId();

        // 获取终止节点
        set<int> getEndNodeId();
        
        // 查找节点, 返回节点索引
        int findNode(int id);
        
        // 添加一条转移函数, 自动新建节点
        int addSrc(int src_id, int dst_id, char transfer_ch);

        //排序
        int sortById();

        // 打印节点和转移函数
        void toString();

        // 查找下一个子集
        set<int> findNextSet(int node_id, char ch);

        void _findNextSet(int node_id);
        
        // 子集构造法装换成DFA
        NFA toDFA();
    };
