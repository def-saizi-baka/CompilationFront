#include "../include/intercode.h"
#include <iostream>
#include <sstream>
#include "../include/symbolTable.h"
#include <fstream>

//extern config con;

#define NOT_FOUND_INDEX -1

symbolTable symTable;

// ��Ԫʽ��
Quadruple::Quadruple(const int cur,const string op, const string left_num, const string right_num, const string res)
{
	this->cur = cur;
	this->op = op;
	this->left_num = left_num;
	this->right_num = right_num;
	this->res = res;
}

ostream& operator<<(ostream& out, const Quadruple& t)
{
	out << to_string(t.cur) << ":";
	out << "(" << t.op << "," << t.left_num << "," << t.right_num << "," << t.res << ")";
	return out; 
}

ofstream& operator<<(ofstream& out, const Quadruple& t)
{
	out << to_string(t.cur) << ":";
	out << "(" << t.op << "," << t.left_num << "," << t.right_num << "," << t.res << ")";
	return out; 
}

// ���ʽԪ��
E::E(string _name):VN()
{
	this->name = _name;
}

E::E(string name, string val) :VN() {
	this->name = name;
	this->value = val;
}

S::S(int nextquad)
{
	this->nextquad = nextquad;
}

// ��������
vector<int>& E::getTrueList()
{
	return this->truelist;
}

// ���ؼ���
vector<int>& E::getFalseList()
{
	return this->falselist;
}

// ���ս�� M 
M::M(int quad) :VN()
{
	this->quad = quad;
}

// ����quad����
int M::getNextquad()
{
	return this->quad;
}

// ���� S.nextlist
vector<int>& S::getNextList()
{
	return this->nextlist;
}

// ���� N.nextlist
vector<int>& N::getNextList()
{
	return this->nextlist;
}

// �м����
InterCode::InterCode()
{
	this->nextquad = 100;	//��ʼ��д��ַ����Ϊ100
	this->pst = nullptr;
}

InterCode::InterCode(parserTree& pst)
{
	this->nextquad = 100;
	this->pst = &pst;
}

// �ϲ�TrueList��FalseList
vector<int> InterCode::merge(const vector<int>&L1,const vector<int>& L2)
{
	vector<int> temp;
	temp.insert(temp.end(), L1.begin(), L1.end());
	temp.insert(temp.end(), L2.begin(), L2.end());
	return temp;
}

// L3 = L1 + L2
void InterCode::merge(const vector<int>& L1, const vector<int>& L2, vector<int>& L3)
{
	if (L3.size() != 0)
	{
		cout << "may error" << endl;
	}
	L3.insert(L3.end(), L1.begin(), L1.end());
	L3.insert(L3.end(), L2.begin(), L2.end());
}

void InterCode::merge(vector<vector<int> >L, vector<int>& res)
{
	if (res.size() != 0)
	{
		cout << "may error" << endl;
	}
	
	for (uint32_t i=0; i<L.size(); i++)
	{
		res.insert(res.end(), L[i].begin(), L[i].end());
	}
}

// ���ݵ�ַ�ҵ���Ӧ����Ԫʽ
int InterCode::findByAddr(int quad)
{
	int index = 0;
	for (auto& iter : this->code)
	{
		if (iter.cur == quad)
		{
			return index;
		}
		index++;
	}
	index = NOT_FOUND_INDEX;
	return index;
}

// ����
void InterCode::backpatch(vector<int>& bpList,int quad)
{
	for (auto& iter : bpList)
	{
		int index = findByAddr(iter);
		code[index].res = to_string(quad);
	}
}

// �����Ԫʽ
void InterCode::emit(Quadruple temp)
{
	code.push_back(temp);
	nextquad++;
}

void InterCode::assignExpression(const node& root, bool bool_flag)
{
	/*
		<��ֵ���> �� <������> <��ֵ����> <��������>
		<��ֵ���> �� <������> <��ֵ����> <���ʽ>
	*/
	string varName = root.kids[0]->value;	// ȡ������

	// ���������Ƿ�δ����
	symbol* it = symTable.look(varName);
	if (it == nullptr) {//�˴�Ϊδ����
		string msg = "symbol " + varName + " is undefined.";
		throw UndefinedDefinitionsException(msg, this->line);
	}

	// ����������ֵ
	if (it->get_const()) {//�ı�Ϊ������
		string msg = "const symbol " + varName + " cannot be assigned";
		throw ConstantModificationException(msg, this->line);
	}

	// �ж���<��������>����<���ʽ>
	if (root.kids[2]->symbol == 1023) {
		// ���ʽ
		E* e = (E*)eleStack.back();
		eleStack.pop_back();
		// ����һ����ֵ���
		emit(Quadruple(nextquad, root.kids[1]->leaf[0]->value, e->value, "_", varName));
		// ����Eѹջ
		// TODO 
		E* e_added = new E("", symTable.get_temp());
		eleStack.push_back(e_added);
	}
	else if (root.kids[2]->symbol == 1017) {
		// ��������δʵ��
	}
}

void InterCode::calExpression(const node& root,bool bool_flag)
{
	// <�������> �� <���ʽ> <�������> <���ʽ>
	E* e_right = (E*)eleStack.back();	// �Ҳ�����
	eleStack.pop_back();
	E* e_left = (E*)eleStack.back();	// �������
	eleStack.pop_back();
	// nextquad ������ ������� �Ҳ����� ��ʱ����
	string temp = symTable.get_temp();
	emit(Quadruple(nextquad, root.kids[1]->leaf[0]->value, e_left->value, e_right->value, temp));
	// ��ǰ���ʽѹջ
	E* e_added = new E("", temp);
	eleStack.push_back(e_added);
}

void InterCode::logicExpression(string logicOp, bool bool_flag)
{
	//string symbol = root.kids[1]->leaf[0]->value;
	if (logicOp == "&&") {
		andExpression();
	}
	else if (logicOp == "||") {
		orExpression();
	}
	else if (logicOp == "!") {
		notExpression();
	}
}

void InterCode::selfChangeExpression(const node& root,bool is_front)
{
	string varName;
	string varOperator;
	if (is_front)
	{
		// <��Ŀ����> �� <��Ŀ������1038><������>
		varName = root.leaf[1]->value;
		varOperator = root.leaf[0]->value;
	}else
	{
		varName = root.leaf[0]->value;
		varOperator = root.leaf[1]->value;
	}

	// δ����
	symbol* it = symTable.look(varName);
	if (it == nullptr) {//�˴�Ϊδ����
		string msg = "symbol " + varName + " is undefined.";
		throw UndefinedDefinitionsException(msg, this->line);
	}

	// ����������ֵ
	if (it->get_const()) {//�ı�Ϊ������
		string msg = "const symbol " + varName + " cannot be assigned";
		throw ConstantModificationException(msg, this->line);
	}
	string temp = symTable.get_temp();
	emit(Quadruple(nextquad, varOperator, "_", varName, temp));
	if (is_front)				// ǰ���������޸�ֵ
	{
		emit(Quadruple(nextquad, ":=", temp,"_", varName));
	}
	E* e_added = new E("", temp);
	eleStack.push_back(e_added);
}

void InterCode::operationExpression(const node& root) {
	/*
		<�������> �� <��Ŀ����>
		<�������> �� <��Ŀ����>
	*/
	const node* now_1 = root.kids[0];
	switch (root.kids[0]->symbol)
	{
		case 1029:	// ��Ŀ����
		{
			/*
				<��Ŀ����> �� <��ֵ���>
				<��Ŀ����> �� <�������>
				<��Ŀ����> �� <��ϵ���>
				<��Ŀ����> �� <�߼����>
			*/
			const node* now_2 = now_1->kids[0];	
			// ����switch�ĸ��ڵ�Ϊ now_2 (��ֵ���1030/�������1032/��ϵ���1047/�߼����1048)
			switch (now_1->kids[0]->symbol)
			{
				case 1030:	// <��Ŀ����1029> �� <��ֵ���1030>
				{
					assignExpression(*now_2);
					break;
				}
				case 1032:  // <��Ŀ����1029> �� <�������1032>
				{
					calExpression(*now_2);
					break;
				}					
				case 1047:	
					// <��Ŀ����> �� <��ϵ���1047>
					// <��ϵ���> �� <���ʽ> <��ϵ����> <���ʽ>
					relopExpression(*now_2);
					break;

				case 1048:	
					// <��Ŀ����> �� <�߼����1048>
					// <�߼����> �� <���ʽ> <�߼�����> <���ʽ>
					logicExpression(now_2->kids[1]->leaf[0]->value);
					break;

				default:
					break;
				}
		}
		break;
		case 1037:	// ��Ŀ����1037 now_1
			/*
				<�������> �� <��Ŀ����1037>
				<��Ŀ����> �� <��Ŀ������1038><������> (�������Ҫ����not)
				<��Ŀ����> �� <������><��Ŀ������>
			*/
		{
			if (now_1->leaf[0]->symbol == con.get_symbols()["++"]
				|| now_1->leaf[0]->symbol == con.get_symbols()["--"])
			{
				selfChangeExpression(*now_1, true);
			}
			// not ����
			else if (now_1->kids[0]->value == "!") {
				notExpression("!");
			}
			else if (now_1->leaf[1]->symbol == con.get_symbols()["++"]
					|| now_1->leaf[1]->symbol == con.get_symbols()["--"])
			{
				selfChangeExpression(*now_1, false);
			}
		}
		break;
	default:
		break;
	}
}

// ������������䷭��
void InterCode::defineConst(const node& root)
{
	vector<node*> leaf = root.leaf;
	string const_str = leaf[0]->value;
	string valType = leaf[1]->value;
	for (uint32_t i = 2; i < leaf.size();) {
		// ��ȡ������
		string valName = leaf[i]->value;
		if (symTable.look(valName) != NULL) {
			// cout << "�ض���ı���: "<< valName << endl;
			string msg = "symbol " + valName + " is multiply defined.";
			throw MultipleDefinitionsException(msg, this->line);
			//exit(-1);
		}
		// ������ = 
		string op_symbol = leaf[i+1]->value;
		// ��ȡֵ;
		// ��ȡ���ʽ��ֵ
		E* e = (E*)eleStack.back();
		eleStack.pop_back();
		// ���ű���������
		symTable.enter(valName, string_type(valType), true);
		// ������Ԫʽ
		this->emit(Quadruple{ nextquad,":=", e->value, "_", valName });
		// ���� = ���沿��
		while (i < leaf.size() && leaf[i]->value != ",") {
			if (leaf[i++]->value == ",") {
				break;
			}
		}
		// ��������
		if (i < leaf.size() && leaf[i]->value == ",") {
			i++;
		}
			
	}

	// ת����S
	S* s_tmp = new S(nextquad);
	eleStack.push_back(s_tmp);
}

// ������䷭�� ��֧�ַǳ���
void InterCode::defineVariable(const node& root)
{
	// ���ӵ�Ҷ�ڵ��Ǳ�������
	const node* defineTree = root.kids[0];
	string valType = defineTree->kids[0]->leaf[0]->value; //int
	vector<string> name_arr;
	vector<string> value_arr;
		// �Һ�����һ�����Ƹ�ֵ���Ķ���
		vector<node*> leaf = defineTree->leaf;
		for (uint32_t i = 1; i < leaf.size();) {
			// ��ȡ������
			string valName = leaf[i]->value;
			if (symTable.look(valName) != NULL) {
				//cout << "�ض���ı���: "<< valName << endl;
				string msg = "symbol " + valName + " is multiply defined.";
				throw MultipleDefinitionsException(msg, this->line);
				//exit(-1);
			}
			if (i+1 >= leaf.size()||leaf[i + 1]->value == ",") { // int b,
				i+=2;
				// ���ű�����һ������
				symTable.enter(valName, string_type(valType), false);
			}
			else if(leaf[i + 1]->value == "=") {	// int a=0, b
				name_arr.push_back(valName);
				// ��ȡ���ʽ��ֵ
				E* e = (E*)eleStack.back();
				eleStack.pop_back();
				value_arr.push_back(e->value);
				
				while (i < leaf.size() && leaf[i]->value != ",") {
					if (leaf[i++]->value == ",") {
						break;
					}
				}
				// ��������
				if (i < leaf.size() && leaf[i]->value == ",") {
					i++;
				}
			}
			else {
				cout << "Debug: ����ʽ����" << endl;
				exit(-1);
			}
		}
		for (auto name : name_arr) {
			string val = value_arr.back();
			value_arr.pop_back();
			// ���ű���������
			symTable.enter(name, string_type(valType), false);
			// ������Ԫʽ
			this->emit(Quadruple{ nextquad,":=", val, "_", name });
		}
	//else {
	//	string valName = defineTree->leaf[1]->value;
	//	if (symTable.look(valName) != NULL) {
	//		//cout << "�ض���ı���: " << valName << endl;
	//		string msg = "symbol " + valName + " is multiply defined.";
	//		throw MultipleDefinitionsException(msg, this->line);
	//		//exit(-1);
	//	}
	//	// ���ű�����һ������
	//	symTable.enter(valName, string_type(valType), false);
	//}

	// ת����S
	S* s_tmp = new S(nextquad);
	eleStack.push_back(s_tmp);

}

// ���ʽ��� -> ���ʽ
void InterCode::expression2statment() 
{
	// ��ȡE
	E* e = (E*)eleStack.back();
	eleStack.pop_back();
	// ת����S
	S* s_tmp = new S(nextquad);
	eleStack.push_back(s_tmp);
}

// M ����
void InterCode::MStatement()
{
	M* tmp = new M(nextquad);
	eleStack.push_back((VN*)tmp);
	//TFStack.push_back(vector<int>(1, cur_addr));
}

// N ����
void InterCode::NStatement()
{
	// TFStack.push_back(vector<int>(1, cur_addr));

	// Quadruple emit(cur_addr++, "j", "-", "-", "-");
	// code.push_back(emit);

	N* tmp = new N();
	tmp->nextlist.push_back(nextquad);
	VN* vp = (VN*)tmp;
	eleStack.push_back((VN*)tmp);

	// ѹջ����ַ��nextlist����
	emit(Quadruple(nextquad, "j", "-", "-", ""));

}

// �����������ʽ�ķ���
void InterCode::valNameExpression(const node& root,bool bool_flag)
{
	// TODO
	string valName = root.leaf[0]->value;

	vector<node*> kids;
	// ���value ��Ӧ���Ǳ���/������
	if (this->pst->get_kids(&root, con.get_symbols()["��ʶ��"], kids)) {
		for (const auto& t : kids) {
			if (t->symbol == con.get_symbols()["������"]) {
				if (symTable.look(valName) == nullptr) {
					string msg = "symbol " + valName + " is undefined.";
					throw UndefinedDefinitionsException(msg, this->line);
				}
				break;
			}
			else if (t->symbol == con.get_symbols()["����"]) {
				break;
			}
		}
	}
	E* e = new E("", valName);
	if (bool_flag) {
		e->truelist.push_back(nextquad);
		e->falselist.push_back(nextquad + 1);
		emit(Quadruple(nextquad, "jnz", valName, "_", "0"));
		emit(Quadruple(nextquad, "j", "_", "_", "0"));
	}
	eleStack.push_back((VN*)e);
}

// && ����
void InterCode::andExpression(bool bool_flag)
{
		// E1 && M E2
		// eleStack ջ��
		// E1 M E2
		int end = eleStack.size();
		auto E2 = eleStack[end - 1];
		auto M = eleStack[end - 2];
		auto E1 = eleStack[end - 3];

		// �˴����޸ģ���Լ���Ӧ�����﷨�����л�ȡ
		// TODO
		auto reduction = new E("xx���ʽ", "");

		// PPT P72
		// ����
		backpatch(E1->getTrueList(), M->getNextquad());

		//  E.truelist = E2.truelist
		reduction->truelist = E2->getTrueList();

		// E.falselist = merge(E1.falselist,E2.falselist)
		merge(E1->getFalseList(), E2->getFalseList(), reduction->falselist);

		// �� E1 M E2 pop��ȥ
		for (int i = 0; i < 3; i++)
		{
			eleStack.pop_back();
		}
		delete E2;
		delete M;
		delete E1;

		eleStack.push_back((VN*)reduction);
}

// || ����
void InterCode::orExpression(bool bool_flag)
{
	// E1 or M E2
	int end = eleStack.size();
	auto E2 = eleStack[end - 1];
	auto M = eleStack[end - 2];
	auto E1 = eleStack[end - 3];

	// PPT 71ҳ
	E* reduction = new E("xx���ʽ","");
	
	backpatch(E1->getFalseList(), M->getNextquad());

	merge(E1->getTrueList(), E2->getTrueList(), reduction->truelist);

	reduction->falselist = E2->getFalseList();

	// ����E1 M E2
	for (int i = 0; i < 3; i++)
	{
		eleStack.pop_back();
	}
	delete E2;
	delete M;
	delete E1;

	eleStack.push_back((VN*)reduction);
}

// !����
void InterCode::notExpression(bool bool_flag)
{
	// E -> not E1
	int end = eleStack.size();
	auto E1 = eleStack[end - 1];

	//PPT 73
	E* reduction = new E("xx���ʽ", "");

	reduction->truelist = E1->getFalseList();

	reduction->falselist = E1->getTrueList();

	eleStack.pop_back();

	delete E1;

	eleStack.push_back((VN*)reduction);
}

// �������ű�ľ���ʵ��
// ʵ�ֻ������ƹ�ϵ���ʽ
void InterCode::relopExpression(const node& root, bool bool_flag)
{
	string eValor = symTable.get_temp();
	E* e_tmp = new E("", eValor);
	e_tmp->truelist.push_back(nextquad);
	e_tmp->falselist.push_back(nextquad + 1);

	E* e_right = (E*)eleStack.back();	// �Ҳ�����
	eleStack.pop_back();
	E* e_left = (E*)eleStack.back();	// �������
	eleStack.pop_back();
	string relopOperator = root.kids[1]->leaf[0]->value;

	string jmp_op = "j"+ relopOperator;

	emit(Quadruple(nextquad, jmp_op, e_left->value, e_right->value,"0"));
	emit(Quadruple(nextquad, "j", "_", "_", "0"));

	eleStack.push_back(e_tmp);
	delete e_right;
	delete e_left;
}

//if E then S1
void InterCode::if_then_statement()
{
	// PPT 84
	// S -> if E then M S1
	int end = eleStack.size();
	auto S1 = eleStack[end - 1];
	auto M = eleStack[end - 2];
	auto E = eleStack[end - 3];

	S* reduction = new S(nextquad);

	backpatch(E->getTrueList(), M->getNextquad());
	merge(E->getFalseList(), S1->getNextList(),reduction->getNextList());

	for (int i = 0; i < 3; i++)
	{
		eleStack.pop_back();
	}
	delete S1;
	delete M;
	delete E;

	eleStack.push_back((VN*)reduction);
}

// if E then M1 S1 N else M2 S2
void InterCode::if_else_statement()
{
	// PPT 84
	int end = eleStack.size();
	auto S2 = eleStack[end - 1];
	auto M2 = eleStack[end - 2];
	auto N  = eleStack[end - 3];
	auto S1 = eleStack[end - 4];
	auto M1 = eleStack[end - 5];
	auto E  = eleStack[end - 6];

	
	S* reduction = new S(nextquad);
	
	backpatch(E->getTrueList(), M1->getNextquad());
	backpatch(E->getFalseList(), M2->getNextquad());

	vector<vector<int> > list;
	list.push_back(S1->getNextList());
	list.push_back(N->getNextList());
	list.push_back(S2->getNextList());

	merge(list, reduction->getNextList());

	for (int i = 0; i < 6; i++)
	{
		eleStack.pop_back();
	}
	delete S2;
	delete M2;
	delete N;
	delete S1;
	delete M1;
	delete E;

	eleStack.push_back((VN*)reduction);
}

// while do
// while M1 E do M2 S1 
void InterCode::while_do_statement()
{
	int end = eleStack.size();
	auto S1 = eleStack[end - 1];
	auto M2 = eleStack[end - 2];
	auto  E = eleStack[end - 3];
	auto M1 = eleStack[end - 4];

	S* reduction = new S(nextquad + 1);
	
	backpatch(S1->getNextList(), M1->getNextquad());
	backpatch(E->getTrueList(), M2->getNextquad());

	reduction->nextlist = E->getFalseList();
	
	stringstream ss;
	ss << M1->getNextquad();
	emit(Quadruple(nextquad,"j", "_", "_",ss.str()));

	for (int i = 0; i < 4; i++)
	{
		eleStack.pop_back();
	}

	delete M2;
	delete S1;
	delete M1;
	delete E;

	eleStack.push_back((VN*)(reduction));
}

// ���ʽ
void InterCode::allExpression(const node& root, bool bool_flag)
{
	/*
		<���ʽ> �� <�������>
		<���ʽ>��<��ʶ��>
		<���ʽ>��<(> <���ʽ> <)>
		<���ʽ>��
	*/
	switch (root.kids[0]->symbol)
	{
		case 1026:	// <���ʽ> �� <�������1026>
			operationExpression(*(root.kids[0]));
			break;
		case 1008:	// <���ʽ>��<��ʶ��1008>
			valNameExpression(root, bool_flag);
			break;
		case 100:	// <���ʽ>��<(> <���ʽ> < )>
			break;
		default:
			break;
	}
}


// ����List
void InterCode::statemmentList(const node& root)
{
	if (root.kids[0]->symbol == con.get_symbols()["���List"])
	{
		int end = eleStack.size();
		auto S1 = eleStack[end - 1];
		auto L = eleStack[end - 2];
		backpatch(L->getNextList(), L->getNextquad());

		S* reduction = new S(S1->getNextquad());

		reduction->nextlist = S1->getNextList();

		eleStack.pop_back();
		eleStack.pop_back();

		delete L;
		delete S1;

		eleStack.push_back((VN*)(reduction));
	}
	else if (root.kids[0]->symbol == con.get_symbols()["���"])
	{
		int end = eleStack.size();
		auto S1 = eleStack[end - 1];
		
		S* reduction = new S(S1->getNextquad());
		reduction->nextlist = S1->getNextList();

		eleStack.pop_back();
		delete S1;
		
		eleStack.push_back((VN*)reduction);
	}
}

// ��������
void InterCode::statementBlock(const node& root)
{
	
	if (root.kids.size() == 2) //< ���� > �� < { > < } >
	{
		S* S1 = new S(nextquad);
		eleStack.push_back((VN*)S1);
	}
	else  // <����> �� <{><���List><}>
	{
		// ����Ҫ���������
	}
}


void InterCode::genCode(const node& root,int line)
{
	this->line = line;
	switch (root.symbol)
	{
		case 1034: // �����������
			defineVariable(root);
			break;
		case 1042:	// �������������
			defineConst(root);
			break;
		case 1023: // ���ʽ����(������ֵ�����㡢��ϵ���߼�����)
			allExpression(root);
			break;
		case 1051:	// �������ʽ
			allExpression(root, true);
			break;
		case 1019:	// ���ʽ���
			expression2statment();
		case 1016:	// ���
			break;
		case 1015:	// ���List
			statemmentList(root);
			break;
		case 1014:	// ����:
			statementBlock(root);
			break;
		case 1020:	// ѡ�����
			if (root.kids.size() > 6)
				if_else_statement();
			else
				if_then_statement();
			break;
		case 1021:  // ѭ�����
			while_do_statement();
			break;
		case 1049:	// M
			MStatement();
			break;
		case 1050:	// N
			NStatement();
			break;
		default:
			return;
	}

	 /*this->outputCode();
	 cout << endl;*/
}


void InterCode::outputCode(const char* filename)
{
	if (filename == NULL)
	{
		cout << "********************************************" << endl;
		cout << "          InterCode                         " << endl;
		int i = code.size() - 1;
		for (uint32_t i = 0; i < code.size();i++)
		{
			cout << code[i] << endl;
		}
		//code = vector<Quadruple>();
		cout << "********************************************" << endl;
	}
	else 
	{
		// ������ļ�
		ofstream fout(filename, ios::out);
		// TODO
		fout << "                 InterCode                  " << endl;
		fout << "********************************************" << endl;
		int i = code.size() - 1;
		for (uint32_t i = 0; i < code.size(); i++)
		{
			fout << code[i] << endl;
		}
		fout << "********************************************" << endl;
		fout.close();
	}
}