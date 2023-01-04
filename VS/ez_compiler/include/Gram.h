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

#define EPSILON			999 // 这里也是偷懒了，缝合的时候要换成符号表的符号
using namespace std;

class CFG;

// 单条语法产生式，注意与Item的区别
class Gram{
    protected:
        int left;
        vector<int> right;
    public:
        // 从配置文件中读取初始文法
        Gram(int left, const vector<int>& right);
        Gram(const Gram& tmp);
        
        friend class CFG;
        friend class Closure;

        void showGram();
};


// 继承了Gram的属性，增加了dotPos等属性
class Item : public Gram{
    private:
        // dosPos表示点存在于项目右侧的哪个位置，例1002 -> .1003 1004,dosPos = 0
        // 当点处于最后位置，将dosPos设置为 -1 
        uint32_t dotPos;                 // 点的位置
        int forward;        // 前线搜索符号
        int type;                   // 当前操作是移进还是规约
        int id;             // 第几条产生式
    public:
        // 实际只初始化了left 和 right
        Item(int left, const vector<int>& right, uint32_t dotPos=0);

        Item(const Gram&, uint32_t dotPos=0);

        // 获取类型
        int getType(){return this->type;}
        int getDotPos(){return this->dotPos;}
        // 获取点后面的一个元素, 自行保证是否越界
        int getDotNext(){return this->right[this->dotPos];}
        int getForward(){return this->forward;}
        int getId(){return this->id;}
        // 获取点后面的所有元素
        vector<int> getDotNextAll();

        void setId(int id){this->id = id;}
        void setForward(int a){this->forward = a;}
        // 点向左移动一格
        void dotRightMove();
        // 从其他Item中创建新的Item，dotPos的移动
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
        vector<Gram> initGram;                  // 初始语法
        vector<Item> allItem;	                // 拓展语法 
        vector<Item> LRItem;				    // LR0项目

        map<string, int> terSysboms;                 // 终结符号集合
        map<string, int> nonTerSysboms;              // 非终结符号集合


        vector<Closure> closures;               // 闭包
        vector< set<pair<int, int>> > closuresRelation;    // 闭包之间的关系, 下标代表第几个比较项目集, 里面的集合中的pair<输入符号, 转移到的闭包集合>

        map<int, vector< pair<int, int> >> analysisTable;

        map<int, set<int> > leftToGramIndex;	// 根据生成式左侧快速锁定对应的生成式
        //map<int, set<int>> firstSet;			// 保存全部非终结符的FIRST集,
        map<int, FIRST> firstSet;
        int begState;							// 初始终结符的状态
        bool debug = false;                     // 调试模式

        void formFirstSet(int value);			// 求取某个符号的非终结符,主要是用于生成firstSet
    public:
        CFG();
        CFG(const string& grammerFile);
        //void expandGrammmer();								//生成拓广文法,个人感觉没必要，直接CFG初始化的时候直接拓广文法就完事了
        void initItems();                                       // 直接CFG初始化的时候直接拓广文法
        void initLRItems();										// 生成所有LR(0)项目
        void formFirstSet();									// 生成所有符号的First集

        void setDebug(){this->debug = true;}                    // 设置输出全部内容
        set<int> getFirstSet(int value);						// 获取单个非终结符的FIRST集
        set<int> getFirstSet(vector<int> gramStr);					// 获取某个串的非终结符
        map<int, std::vector<std::pair<int, int>>> getAnalysisTable(){return this->analysisTable;}
        Item getInitItem();                                     // 获取项目初始项
        void showCFG();
        void showFirstSet();
        void buildClosures();                                    // 构建闭包
        void buildAnalysisTable();                               // 构建action和goto表
        
        void save(string path = "analysisTableModel");
        void load(string path = "analysisTableModel");
        void load(bool simpleLoad, string path = "analysisTableModel");// select决定是否要进行直接加载,true为直接加载，必须提供
        friend class Closure;
};

class Closure{
    private:
        set<Item>family;
    public:
        // 初始化一个闭包, 并进行扩展
        Closure(CFG& cfg,const set<Item>& itemSet);
        // 该构造函数不会进行扩展
        Closure(set<Item>& itemSet);
        set<Item> GO(int input);

        const set<Item>& getFamily(){return this->family;}

        void printClosure();
        void showItem(Item& item);

        bool operator==(const Closure& other) const;
};

#endif