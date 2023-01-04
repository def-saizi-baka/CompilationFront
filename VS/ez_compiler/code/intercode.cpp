#include "../include/intercode.h"
#include <iostream>
#include <sstream>
#include "../include/symbolTable.h"

//extern config con;
// �˴��궨��û���ã������ֹ�������ʱ����������ȥ
// ��Լ������Ӧ�ô�Parse���ж�ȡ��̬����
#define DEFINE_CONST 0
#define DEFINE_VARIABLE 1
#define ASSIGNMENT 2
#define BOOL_EXPR 3

#define NOT_FOUND_INDEX -1

symbolTable sysTable;

// ��Ԫʽ��
Quadruple::Quadruple(const int cur,const string op, const string left_num, const string right_num, const string res)
{
	this->cur = cur;
	this->op = op;
	this->left_num = left_num;
	this->right_num = right_num;
	this->res = res;
}


VN::VN()
{
	this->type = 0;
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

// ˵����䷭��
void InterCode::defineConst()
{
	// TODO
	// const a = 1;
	// oprand = {a,1}
	// ���ö�Ӧ�ķ��ű�ӿ�
	// 
	// ��ֹ�ض���
}

void InterCode::operationStatement(const node& root) {
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
			*/
			const node* now_2 = now_1->kids[0];	// ���ڸ��ڵ�Ϊ now_2 (��ֵ���1030)
			switch (now_1->kids[0]->symbol)
			{
				case 1030:	// <��Ŀ����1029> �� <��ֵ���1030>
				{
					/*
					<��ֵ���> �� <������> <��ֵ����> <��������>
					<��ֵ���> �� <������> <��ֵ����> <���ʽ>
					*/
					string varName = now_2->kids[0]->value;	// ȡ������
					// �ж��� <��������>����<���ʽ>
					if (now_2->kids[2]->symbol == 1023) {
						// ���ʽ
						E* e = (E*)eleStack.back();
						eleStack.pop_back();
						// ����һ����ֵ���
						emit(Quadruple(nextquad, ":=", varName, e->value, "T?"));
						// ����Eѹջ
						E* e_added = new E("", "T?");
						eleStack.push_back(e_added);
					}
					else if(now_2->kids[2]->symbol == 1017) {
						// ��������δʵ��
					}
					else {
						// δ֪����
					}
					break;
				}
				
				case 1032:  // // <��Ŀ����1029> �� <�������>
				{
					// <�������> �� <���ʽ> <�������> <���ʽ>
					E* e_left = (E*)eleStack.back();	// �Ҳ�����
					eleStack.pop_back();
					E* e_right = (E*)eleStack.back();	// �������
					eleStack.pop_back();
					// nextquad ������ ������� �Ҳ����� ��ʱ����
					emit(Quadruple(nextquad, now_2->kids[1]->value, e_left->value, e_right->value, "T?"));
					// ��ǰ���ʽѹջ
					E* e_added = new E("", "T?");
					eleStack.push_back(e_added);
					break;
				}					
				default:
					break;
				}
		}


		break;
		case 1037:	// ��Ŀ����1037 now_1
			/*
				<�������> �� <��Ŀ����1037>
				<��Ŀ����> �� <��Ŀ������1038><������>
				<��Ŀ����> �� <������><��Ŀ������>
			*/
		{
			if (now_1->kids[0]->symbol == 1038) {
				// <��Ŀ����> �� <��Ŀ������1038><������>
				string varName = now_1->leaf[0]->value;
				string varOperator = now_1->leaf[1]->value;
				emit(Quadruple(nextquad, varOperator, "", varName, "T?"));
				E* e_added = new E("", "T?");
				eleStack.push_back(e_added);
			}
			else{
				// <��Ŀ����> �� <������><��Ŀ������>
				string varName = now_1->leaf[1]->value;
				string varOperator = now_1->leaf[0]->value;
				emit(Quadruple(nextquad, varOperator, varName, "", "T?"));
				E* e_added = new E("", "T?");
				eleStack.push_back(e_added);
			}
		}
		break;
	default:
		break;
	}
}

// ������䷭�� ��֧�ַǳ���
void InterCode::defineVariable(const node& root)
{
	// ���ӵ�Ҷ�ڵ��Ǳ�������
	const node* defineTree = root.kids[0];
	string valType = defineTree->kids[0]->leaf[0]->value; //int

	if (defineTree->leaf.size() > 2) {
		// �Һ�����һ�����Ƹ�ֵ���Ķ���
		const node* assignmentTree = defineTree->kids[1]->kids[0];
		string valName = assignmentTree->kids[0]->value;

		if (sysTable.look(valName) != NULL) {
			cout << "�ض���ı���: "<< valName << endl;
			exit(-1);
		}

		// ��ȡ���ʽ��ֵ
		E* e = (E*)eleStack.back();
		eleStack.pop_back();
		// ���ű���������
		sysTable.enter(valName, string_type(valName), false);
		this->emit(Quadruple{ nextquad,":=", e->value, "", valName });
	}

	else {
		string valName = defineTree->leaf[1]->value;
		if (sysTable.look(valName) != NULL) {
			cout << "�ض���ı���: " << valName << endl;
			exit(-1);
		}
		// ���ű�����һ������
		sysTable.enter(valType, string_type(valName), false);
	}

	// int a = 1;
	// operand = {a,1}
	// ��ֹ�ض���
}

// ��ֵ��䷭��
void InterCode::assignStatement()
{
	// �������иú��������ڱ��ʽ��������ʵ�ָ�ֵ
	// operand
	// TODO

	// ������Ԫʽ
	
	// ����Ҫ���ݱ��ʽ�����
	// a = b
	// (=,b,-,a)
	// a = b + c
	// (=,b,c,a)

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
	VN* vp = (VN*)tmp;
	eleStack.push_back((VN*)tmp);

	// ѹջ����ַ��nextlist����
	emit(Quadruple(nextquad, "j", "-", "-", ""));

}


// �������ʽ�����ܿس���
void InterCode::boolExpr()
{
	
}

void InterCode::andStatement()
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
	auto reduction = new E("xx���ʽ","");
	

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


void InterCode::orStatement()
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


void InterCode::notStatement()
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
void InterCode::relopStatement()
{
	
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

	S* reduction = new S();

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

	
	S* reduction = new S();
	
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
	// �����ǲ���û���� whileҲ�ǣ�

	eleStack.push_back((VN*)reduction);
}

// while do
void InterCode::while_do_statement()
{
	int end = eleStack.size();
	auto S1 = eleStack[end - 1];
	auto M2 = eleStack[end - 2];
	auto  E = eleStack[end - 3];
	auto M1 = eleStack[end - 4];

	S* reduction = new S();
	
	backpatch(S1->getNextList(), M1->getNextquad());
	backpatch(E->getTrueList(), M2->getNextquad());

	reduction->nextlist = E->getFalseList();
	
	stringstream ss;
	ss << M1->getNextquad();
	emit(Quadruple(nextquad,"j", "-", "-",ss.str()));

	for (int i = 0; i < 4; i++)
	{
		eleStack.pop_back();
	}

	delete M2;
	delete S1;
	delete M1;
	delete E;
}

// ������ʽ��
void InterCode::expression_statement(const node& root) {
	/* �������� */ 
	/*
		<���ʽ> �� <�������>
		<���ʽ>��<��ʶ��>
		<���ʽ>��<(> <���ʽ> <)>
	*/
	switch (root.kids[0]->symbol)
	{
		case 1026:	// <���ʽ> �� <�������1026>
			operationStatement(*(root.kids[0]));
			break;
		case 1008:	// <���ʽ>��<��ʶ��1008>
		{
			string valName = root.leaf[0]->value;
			E* e = new E("", valName);
			eleStack.push_back((VN*)e);
		}
			break;
		case 100:	// <���ʽ>��<(> <���ʽ> < )>
			break;
		default:
			break;
	}
}


void InterCode::genCode(const node& root)
{
	switch (root.symbol)
	{
		case 1034: // �����������
			defineVariable(root);
			break;
		case 1023: // ���ʽ
			expression_statement(root);
			break;

		case DEFINE_CONST:
			defineConst();
			break;
		case ASSIGNMENT:
			assignStatement();
		case BOOL_EXPR:
			boolExpr();
		default:
			return;
	}
}