#pragma once
#include <vector>
#include <string>
#include "config.h"
#include "json/json.h"
extern config con;
using namespace std;


struct node{
	int symbol = -1;//默认-1代表没有符号
	int num = 0;
	vector<node*> kids;
};

class parserTree
{
public:
	void in(int symbol); //移进
	void reduction(pair<int, vector<int>> grammar);//归约
	void end();
	void to_json(string name);
	void traverse(node* root, void (*visit)(int symbol));
	Json::Value build_tree(node* tree);
	~parserTree();
private:
	vector<node*> roots;
	node* root = nullptr;
};

/// <summary>
/// 移进，就把一个节点放到节点栈里面，这里用vector代替栈来使用了
/// </summary>
/// <param name="symbol"></param> 移进的符号的唯一标识符
void parserTree::in(int symbol)
{
	try {
		node* root = new node;
		root->symbol = symbol;
		this->roots.push_back(root);
	}
	catch (const bad_alloc& e) {
		con.log(e.what());
	}
}
/// <summary>
/// 归约，完成语法树的变化
/// </summary>
/// <param name="grammar"></param>参数是一个语法生成式 
void parserTree::reduction(pair<int, vector<int>> grammar)
{
	try {
		if(grammar.second.size() < 1){
			string message = "[ERROR] 语法生成式为空,该语法生成式为：";
			message += to_string(grammar.first) + " → ";
				for (const auto& t : grammar.second) {
					message += to_string(t);
					message += "|";
				}
				if (*message.rbegin() == '|')
					message.pop_back();
				con.log(message);
			con.log(message);
			return ;
		}
		node* root = new node;
		root->symbol = grammar.first;
		for (int i = grammar.second.size() - 1; i >= 0; i--) {
			node* temp = this->roots.back();

			if (grammar.second[i] == temp->symbol) {//这里就是对应上了
				this->roots.pop_back();
				root->num++;
				root->kids.insert(root->kids.begin(), temp);
			}
			else {
				string message = "[ERROR] 生成语法分析树过程中，发现需归约符号和语法生成式不匹配,该语法生成式为：";
				message += to_string(grammar.first) + " → ";
				for (const auto& t : grammar.second) {
					message += to_string(t);
					message += "|";
				}
				if (*message.rbegin() == '|')
					message.pop_back();
				con.log(message);
			}
		}
		this->roots.push_back(root);
	}
	catch (const bad_alloc& e) {
		con.log(string("[ERROR] ") + e.what());
	}
}

/// <summary>
/// 该函数的作用是确定归约成功，并且做一下判断
/// </summary>
void parserTree::end()
{
	if (this->roots.size() == 1) {
		this->root = this->roots[0];
	}
	else { //这样就说明没有归约成一棵语法树
		con.log("[ERROR] 没有成功归约成一个语法树，可能是输入文法有误等问题，请检查前面的报错");
	}
}

static int saveToFile(const string& file_name, const Json::Value& value)
{
	ofstream ofs; //标准输出流
	ofs.open(file_name,ios::out); //创建文件
	if (!ofs.is_open())
	{
		con.log("[ERROR] 未能创建json文件.");
		return -1;
	}
	ofs << value.toStyledString(); //输出
	ofs.close();
	return 0;
}


/// <summary>
/// 将树转化为json格式
/// </summary>
void parserTree::to_json(string name)
{
	if (root == nullptr) {
		con.log("[ERROR] 语法分析树为空，无法构建树");
		return;
	}
	Json::Value value = build_tree(this->root);
	con.log(value.toStyledString());
	if (saveToFile(name, value) != 0)
		con.log("[ERROR] 保存为json文件失败.");
	else
		con.log("[INFO] 保存为json文件成功.");
}

void parserTree::traverse(node* root, void (*visit)(int symbol))
{
	//首先访问根节点
	visit(root->symbol);
	for (const auto& t : root->kids) {
		traverse(t, visit);
	}
}

Json::Value parserTree::build_tree(node* tree)
{
	Json::Value jv_node;
	string tag_name = con.get__symbols()[tree->symbol];
	
	jv_node["0.int"] = tree->symbol;
	jv_node["2.kind"] = tag_name;
	if(tree == this->root)
		jv_node["1.is_root"] = 0; //根节点
	else if(tree->kids.size() == 0)
		jv_node["1.is_root"] = 2;//叶子节点
	else
		jv_node["1.is_root"] = 1; //子树根节点
	con.log("[INFO] 成功展开节点，节点类型为：" + tag_name);
	
	for (size_t i = 0; i < tree->kids.size(); i++)
	{
		node* subtree = tree->kids[i];
		if (subtree != nullptr) {
			Json::Value jv_son = build_tree(subtree);
			jv_node["3.inner"].append(Json::Value(jv_son));
		}
	}
	return jv_node;
}


parserTree::~parserTree()
{
	this->root = nullptr;
	for (auto it = this->roots.begin(); it != this->roots.end(); it++) {
		if ((*it) != nullptr)
			delete (*it);
	}
}


