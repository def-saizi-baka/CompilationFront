#pragma once
#include <vector>
#include <string>
#include "config.hpp"
#include "json/json.h"
extern config con;
using namespace std;

struct node{
	int symbol = -1;//Ĭ��-1����û�з���
	int num = 0;
	vector<node*> kids;
};

class parserTree
{
public:
	void in(int symbol); //�ƽ�
	void reduction(pair<int, vector<int>> grammar);//��Լ
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
/// �ƽ����Ͱ�һ���ڵ�ŵ��ڵ�ջ���棬������vector����ջ��ʹ����
/// </summary>
/// <param name="symbol"></param> �ƽ��ķ��ŵ�Ψһ��ʶ��
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
/// ��Լ������﷨���ı仯
/// </summary>
/// <param name="grammar"></param>������һ���﷨����ʽ 
void parserTree::reduction(pair<int, vector<int>> grammar)
{
	try {
		node* root = new node;
		root->symbol = grammar.first;
		for (size_t i = grammar.second.size() - 1; i >= 0; i--) {
			node* temp = this->roots.back();

			if (grammar.second[i] == temp->symbol) {//������Ƕ�Ӧ����
				this->roots.pop_back();
				root->num++;
				root->kids.insert(root->kids.begin(), root);
			}
			else {
				string message = "[ERROR] �����﷨������ \
					�����У��������Լ���ź��﷨����ʽ��ƥ��,���﷨����ʽΪ��";
				message += to_string(grammar.first) + " �� ";
				for (const auto& t : grammar.second) {
					message += to_string(t);
					message += "|";
				}
				if (*message.rbegin() == '|')
					message.pop_back();
				con.log(message);
			}
		}
	}
	catch (const bad_alloc& e) {
		con.log(string("[ERROR] ") + e.what());
	}
}

/// <summary>
/// �ú�����������ȷ����Լ�ɹ���������һ���ж�
/// </summary>
void parserTree::end()
{
	if (this->roots.size() == 1) {
		this->root = this->roots[0];
	}
	else { //������˵��û�й�Լ��һ���﷨��
		con.log("[ERROR] û�гɹ���Լ��һ���﷨���������������ķ����������\
			������ǰ��ı���");
	}
}

static int saveToFile(const string& file_name, const Json::Value& value)
{
	ofstream ofs; //��׼�����
	ofs.open(file_name); //�����ļ�
	if (!ofs.is_open())
	{
		con.log("[ERROR] δ�ܴ���json�ļ�.");
		return -1;
	}
	ofs << value.toStyledString(); //���
	ofs.close();
	return 0;
}


/// <summary>
/// ����ת��Ϊjson��ʽ
/// </summary>
void parserTree::to_json(string name)
{
	if (root == nullptr) {
		con.log("[ERROR] �﷨������Ϊ�գ��޷�������");
		return;
	}
	Json::Value value = build_tree(this->root);
	con.log(value.toStyledString());
	if (saveToFile(name, value) != 0)
		con.log("[ERROR] ����Ϊjson�ļ�ʧ��.");
	else
		con.log("[INFO] ����Ϊjson�ļ��ɹ�.");
}

void parserTree::traverse(node* root, void (*visit)(int symbol))
{
	//���ȷ��ʸ��ڵ�
	visit(root->symbol);
	for (const auto& t : root->kids) {
		traverse(t, visit);
	}
}

Json::Value parserTree::build_tree(node* tree)
{
	Json::Value jv_node;
	string tag_name = con.get__symbols()[tree->symbol];
	jv_node["kind"] = tag_name;
	for (size_t i = 0; i < tree->kids.size(); i++)
	{
		node* subtree = tree->kids[i];
		if (subtree != nullptr) {
			Json::Value jv_son = build_tree(subtree);
			jv_node["inner"].append(Json::Value(jv_son));
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


