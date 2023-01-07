#include "../include/parserTree.h"


/// <summary>
/// �ƽ����Ͱ�һ���ڵ�ŵ��ڵ�ջ���棬������vector����ջ��ʹ����
/// </summary>
/// <param name="symbol"></param> �ƽ��ķ��ŵ�Ψһ��ʶ��
void parserTree::in(string value, int symbol)
{
	try {
		node* root = new node;
		root->symbol = symbol;
		root->value = value;
		root->leaf.push_back(root);
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
		if (grammar.second.size() < 1) {
			string message = "[ERROR] �﷨����ʽΪ��,���﷨����ʽΪ��";
			message += to_string(grammar.first) + " �� ";
			for (const auto& t : grammar.second) {
				message += to_string(t);
				message += "|";
			}
			if (*message.rbegin() == '|')
				message.pop_back();
			con.log(message);
			con.log(message);
			return;
		}
		node* root = new node;
		root->symbol = grammar.first;
		for (int i = (int)grammar.second.size() - 1; i >= 0; i--) {
			node* temp = this->roots.back();

			if (grammar.second[i] == temp->symbol) {//������Ƕ�Ӧ����
				this->roots.pop_back();
				root->num++;
				temp->father = root;//�趨���ڵ�
				root->kids.insert(root->kids.begin(), temp);
				// ������vector�ĺϲ����������ڵ����Ϣ��������ԭʼ������Ϣ��
				root->leaf.insert(root->leaf.begin(), temp->leaf.begin(), temp->leaf.end()); // ͷ��������Ҷ�ڵ���Ϣ
			}
			else {
				string message = "[ERROR] �����﷨�����������У��������Լ���ź��﷨����ʽ��ƥ��,���﷨����ʽΪ��";
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
		this->roots.push_back(root);
		// gencode

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
		con.log("[ERROR] û�гɹ���Լ��һ���﷨���������������ķ���������⣬����ǰ��ı���");
	}
}

static int saveToFile(const string& file_name, const Json::Value& value)
{
	ofstream ofs; //��׼�����
	ofs.open(file_name, ios::out); //�����ļ�
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

	jv_node["0.int"] = tree->symbol;
	jv_node["2.kind"] = tag_name;
	if (tree == this->root)
		jv_node["1.is_root"] = 0; //���ڵ�
	else if (tree->kids.size() == 0)
		jv_node["1.is_root"] = 2;//Ҷ�ӽڵ�
	else
		jv_node["1.is_root"] = 1; //�������ڵ�
	jv_node["4.value"] = tree->value;
	con.log("[INFO] �ɹ�չ���ڵ㣬�ڵ�����Ϊ��" + tag_name);

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

/// <summary>
/// �ú���ֻ������Ŀǰջ������˵Ĺ�Լ�ڵ��Ҷ�ڵ�����
/// </summary>
/// <returns></returns>
vector<node*> null_node;	// �սڵ㣬��Ϊ����warning
const vector<node*>& parserTree::get_back_leaf()
{
	if (this->roots.size() == 0) {
		con.log("[ERROR] ��ǰ��δ��ʼ��Լ�����ɻ�ȡ��ǰջ��������Ҷ�ڵ�");
		return null_node;
	}
	return this->roots.back()->leaf;
}

//ͨ��������������Եõ�ĳ�����Ӹ��ڵ���Ƶ�����
bool parserTree::get_back_reduction_list(vector<node*>& res)
{
	if (res.size() != 0) {
		con.log("[INFO] ����Ĵ��޸ĵĽڵ������ǿգ�����������Լ�������");
		res.clear(); // ��ո�����
	}
	auto tmp = this->roots.back();
	bool is_one = false;
	if (tmp->leaf.size() != 1) {
		con.log("[INFO] ��õ�Ŀǰջ���ڵ��Ҷ�ڵ㲻ֹһ�����Լ������У����������Ҷ�ڵ�����Ϲ�Լ����");
		is_one = true;
	}
	for (auto& t : tmp->kids) {
		while (t != nullptr && t->symbol != tmp->symbol) {
			res.push_back(t);
			t = t->father;
		}
	}
	return is_one;
}

bool parserTree::get_kids(const node* tree, int symbol, vector<node*>&kids)
{
	std::queue<const node*>q;
	q.push(tree);
	while (!q.empty()) {
		const node* p = q.front();
		q.pop();
		if (p->symbol == symbol) {
			kids = p->kids;
			return true;
		}
		for (const auto& t : p->kids) {
			q.push(t);
		}
	}
	return false;
}

parserTree::~parserTree()
{
	this->root = nullptr;
	for (auto it = this->roots.begin(); it != this->roots.end(); it++) {
		if ((*it) != nullptr)
			delete (*it);
	}
}
