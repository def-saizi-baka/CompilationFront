#pragma once
#ifndef ITEM_H
#define ITEM_H

#include <set>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <iostream>
#include <string>
#include "config.h"

#define ACTION_REDUCE	0
#define ACTION_MOVE		1

#define EPSILON			999 // ����Ҳ��͵���ˣ���ϵ�ʱ��Ҫ���ɷ��ű�ķ���
using namespace std;

class CFG;

// �����﷨����ʽ��ע����Item������
class Gram{
    protected:
        int left;
        vector<int> right;
    public:
        // �������ļ��ж�ȡ��ʼ�ķ�
        Gram(int left, const vector<int>& right);
        Gram(const Gram& tmp);
        
        friend class CFG;
        friend class Closure;

        void showGram();
};


// �̳���Gram�����ԣ�������dotPos������
class Item : public Gram{
    private:
        // dosPos��ʾ���������Ŀ�Ҳ���ĸ�λ�ã���1002 -> .1003 1004,dosPos = 0
        // ���㴦�����λ�ã���dosPos����Ϊ -1 
        uint32_t dotPos;                 // ���λ��
        int forward;        // ǰ����������
        int type;                   // ��ǰ�������ƽ����ǹ�Լ
        int id;             // �ڼ�������ʽ
    public:
        // ʵ��ֻ��ʼ����left �� right
        Item(int left, const vector<int>& right, uint32_t dotPos=0);

        Item(const Gram&, uint32_t dotPos=0);

        // ��ȡ����
        int getType(){return this->type;}
        int getDotPos(){return this->dotPos;}
        // ��ȡ������һ��Ԫ��, ���б�֤�Ƿ�Խ��
        int getDotNext(){return this->right[this->dotPos];}
        int getForward(){return this->forward;}
        int getId(){return this->id;}
        // ��ȡ����������Ԫ��
        vector<int> getDotNextAll();

        void setId(int id){this->id = id;}
        void setForward(int a){this->forward = a;}
        // �������ƶ�һ��
        void dotRightMove();
        // ������Item�д����µ�Item��dotPos���ƶ�
        // Item(const Item&);

        void showItem();

        bool operator == (const Item& other) const;
        bool operator != (const Item& other) const;
        bool operator < (const Item& other) const;
};

class FIRST
{
private:
	set<int> terminals;
	bool sure;
public:
	friend CFG;
	FIRST();
	void insert(int terminal);
	void insert(const FIRST&);
	void divEpsilon();
	int count(int);
	void showFIRST();
    void transSure(bool subSure);
	void transSure(const FIRST&);
	void makeSure();	
	void notSure();
	bool isSure();

};


class Closure;

class CFG{
    private:
        vector<Gram> initGram;                  // ��ʼ�﷨
        vector<Item> allItem;	                // ��չ�﷨ 
        vector<Item> LRItem;				    // LR0��Ŀ

        map<string, int> terSysboms;                 // �ս���ż���
        map<string, int> nonTerSysboms;              // ���ս���ż���


        vector<Closure> closures;               // �հ�
        vector< set<pair<int, int>> > closuresRelation;    // �հ�֮��Ĺ�ϵ, �±����ڼ����Ƚ���Ŀ��, ����ļ����е�pair<�������, ת�Ƶ��ıհ�����>

        map<int, vector< pair<int, int> >> analysisTable;

        map<int, set<int> > leftToGramIndex;	// ��������ʽ������������Ӧ������ʽ
        //map<int, set<int>> firstSet;			// ����ȫ�����ս����FIRST��,
        map<int, FIRST> firstSet;
        int begState;							// ��ʼ�ս����״̬
        bool debug = false;                     // ����ģʽ

        void formFirstSet(int value);			// ��ȡĳ�����ŵķ��ս��,��Ҫ����������firstSet
    public:
        CFG();
        CFG(const string& grammerFile);
        //void expandGrammmer();								//�����ع��ķ�,���˸о�û��Ҫ��ֱ��CFG��ʼ����ʱ��ֱ���ع��ķ���������
        void initItems();                                       // ֱ��CFG��ʼ����ʱ��ֱ���ع��ķ�
        void initLRItems();										// ��������LR(0)��Ŀ
        void formFirstSet();									// �������з��ŵ�First��

        void setDebug(){this->debug = true;}                    // �������ȫ������
        set<int> getFirstSet(int value);						// ��ȡ�������ս����FIRST��
        set<int> getFirstSet(vector<int> gramStr);					// ��ȡĳ�����ķ��ս��
        map<int, std::vector<std::pair<int, int>>> getAnalysisTable(){return this->analysisTable;}
        Item getInitItem();                                     // ��ȡ��Ŀ��ʼ��
        void showCFG();
        void showFirstSet();
        void buildClosures();                                    // �����հ�
        void buildAnalysisTable();                               // ����action��goto��
        
        void save(string path = "analysisTableModel");
        void load(string path = "analysisTableModel");
        void load(bool simpleLoad, string path = "analysisTableModel");// select�����Ƿ�Ҫ����ֱ�Ӽ���,trueΪֱ�Ӽ��أ������ṩ
        friend class Closure;
};

class Closure{
    private:
        set<Item>family;
    public:
        // ��ʼ��һ���հ�, ��������չ
        Closure(CFG& cfg,const set<Item>& itemSet);
        // �ù��캯�����������չ
        Closure(set<Item>& itemSet);
        set<Item> GO(int input);

        const set<Item>& getFamily(){return this->family;}

        void printClosure();
        void showItem(Item& item);

        bool operator==(const Closure& other) const;
};

#endif