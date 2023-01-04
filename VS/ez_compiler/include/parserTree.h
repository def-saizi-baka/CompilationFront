#pragma once
#include <vector>
#include <string>
#include "config.h"
#include "json/json.h"
extern config con;
using namespace std;


struct node {
	int symbol = -1;//默认-1代表没有符号
	string value;
	int num = 0;
	vector<node*> kids;
	vector<node*> leaf;
	node* father = nullptr;
};

class parserTree
{
public:
	void in(string value, int symbol); //移进
	void reduction(pair<int, vector<int>> grammar);//归约
	void end();
	void to_json(string name);
	void traverse(node* root, void (*visit)(int symbol));
	Json::Value build_tree(node* tree);
	const vector<node*>& get_back_leaf();
	bool get_back_reduction_list(vector<node*>& res);
	vector<node*> roots;
	~parserTree();
private:
	
	node* root = nullptr;
};


