#pragma once
#include <vector>
#include <string>
#include <queue>
#include "config.h"
#include "json/json.h"
extern config con;
using namespace std;


struct node {
	int symbol = -1;//Ĭ��-1����û�з���
	string value;
	int num = 0;
	vector<node*> kids;
	vector<node*> leaf;
	node* father = nullptr;
};

class parserTree
{
public:
	void in(string value, int symbol); //�ƽ�
	void reduction(pair<int, vector<int>> grammar);//��Լ
	void end();
	void to_json(string name);
	void traverse(node* root, void (*visit)(int symbol));
	Json::Value build_tree(node* tree);
	const vector<node*>& get_back_leaf();
	bool get_back_reduction_list(vector<node*>& res);
	bool get_kids(const node* tree, int symbol, vector<node*>&kids);//tree���������symbol�ĺ���
	vector<node*> roots;
	~parserTree();
private:
	
	node* root = nullptr;
};


