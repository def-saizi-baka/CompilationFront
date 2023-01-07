#include "../include/parserTree.h"


/// <summary>
/// 移进，就把一个节点放到节点栈里面，这里用vector代替栈来使用了
/// </summary>
/// <param name="symbol"></param> 移进的符号的唯一标识符
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
/// 归约，完成语法树的变化
/// </summary>
/// <param name="grammar"></param>参数是一个语法生成式 
void parserTree::reduction(pair<int, vector<int>> grammar)
{
	try {
		if (grammar.second.size() < 1) {
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
			return;
		}
		node* root = new node;
		root->symbol = grammar.first;
		for (int i = (int)grammar.second.size() - 1; i >= 0; i--) {
			node* temp = this->roots.back();

			if (grammar.second[i] == temp->symbol) {//这里就是对应上了
				this->roots.pop_back();
				root->num++;
				temp->father = root;//设定父节点
				root->kids.insert(root->kids.begin(), temp);
				// 做两个vector的合并，保留根节点的信息（即保留原始符号信息）
				root->leaf.insert(root->leaf.begin(), temp->leaf.begin(), temp->leaf.end()); // 头插入所有叶节点信息
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
		// gencode

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
	ofs.open(file_name, ios::out); //创建文件
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
	if (tree == this->root)
		jv_node["1.is_root"] = 0; //根节点
	else if (tree->kids.size() == 0)
		jv_node["1.is_root"] = 2;//叶子节点
	else
		jv_node["1.is_root"] = 1; //子树根节点
	jv_node["4.value"] = tree->value;
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

/// <summary>
/// 该函数只允许获得目前栈里面最顶端的归约节点的叶节点序列
/// </summary>
/// <returns></returns>
vector<node*> null_node;	// 空节点，仅为消除warning
const vector<node*>& parserTree::get_back_leaf()
{
	if (this->roots.size() == 0) {
		con.log("[ERROR] 当前还未开始归约，不可获取当前栈顶子树的叶节点");
		return null_node;
	}
	return this->roots.back()->leaf;
}

//通过这个函数，可以得到某子树从根节点的推导序列
bool parserTree::get_back_reduction_list(vector<node*>& res)
{
	if (res.size() != 0) {
		con.log("[INFO] 输入的待修改的节点容器非空，清空容器，仍继续运行");
		res.clear(); // 拆空该序列
	}
	auto tmp = this->roots.back();
	bool is_one = false;
	if (tmp->leaf.size() != 1) {
		con.log("[INFO] 获得的目前栈顶节点的叶节点不止一个，仍继续运行，求解这所有叶节点的向上归约序列");
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
