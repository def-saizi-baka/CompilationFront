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

// ��#����yiboxilong
#define S3S '#'

// ��ȡ�ļ��ĺ���
void get_argv(int& src, int& dst, char &transfer_ch,string input);

/* ת�ƺ���(��) */
class Vsrc{
    private:
        int idx;    // ת�ƽڵ�id
        char ch;    // ת���ַ�
    public:
        // ���캯��
        Vsrc(int id, int _ch){ idx = id; ch = _ch;}
        // ��ȡ
        int getId(){return this->idx;}
        char getCh(){return this->ch;}
};


/* NFA�е�ÿһ���ڵ� */
class VNode{
    private:
        int id;
        vector<Vsrc> nextNode;
    public:
        bool is_begin=false;
        bool is_end=false;

        // ���캯��
        VNode(int _id, int _dst, char _ch);
        VNode(int _id){ id = _id; }

        // ��ȡ�ڵ�id
        int getId(){return this->id;};

        // ���һ����
        void addNext(int _dst, char _ch);

        // ��� / ��ӡ�����ڵ�ֵ, ת�ƺ���
        void toString();

        //  ��д�ȽϺ���
        bool operator < (VNode &b){ return this->id < b.getId(); }

        // ������һ������(ֻ����һ��ת��)
        set<int> nextSet(char trans_ch);
};

/* NFA��ʵ�� */
class NFA{
    private:
        vector<VNode> nodeArray;    // �ڵ��б�
        vector<char> symbolTable;   // ���ű�
        set<int> nextAny;           // ����������ת�Ƶĵݹ��ж�
    public:

        // ������ʼ�ڵ�
        int setInitNode(int id);

        // ������ֹ�ڵ�
        int setEndNode(int id);

        // ��ȡ��ʼ�ڵ� / ����
        int getInitNodeId();

        // ��ȡ��ֹ�ڵ�
        set<int> getEndNodeId();
        
        // ���ҽڵ�, ���ؽڵ�����
        int findNode(int id);
        
        // ���һ��ת�ƺ���, �Զ��½��ڵ�
        int addSrc(int src_id, int dst_id, char transfer_ch);

        //����
        int sortById();

        // ��ӡ�ڵ��ת�ƺ���
        void toString();

        // ������һ���Ӽ�
        set<int> findNextSet(int node_id, char ch);

        void _findNextSet(int node_id);
        
        // �Ӽ����취װ����DFA
        NFA toDFA();
    };
