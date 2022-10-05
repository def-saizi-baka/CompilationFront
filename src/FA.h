#include <vector>
#include <string>
#include <map>
#include <stack>
using namespace std;


#define Epsilon_CH '\0'
#define NO_Trans	1


class FA;
class DFA;

class VNode 
{
private:
	int idx;
	bool isEnd;
	map<char, vector<int>> arcs;
	friend FA;
	friend DFA;
public:
	VNode(int idx);
	void setEdge(char ch, int idx);
};

class FA
{
protected:
	int begNode;
	int endNode;
	vector<VNode> mgraph;

	static map<char, int> pri_op;
	void mergerFA(stack<FA>& tmp, char op);
public:
	FA();
	FA(char ch);
	FA(string reg);
	
	void mergeFAbyOr(FA& tmp);
	void mergeFAbyLink(FA& tmp);
	void mergeFAbyClosure();

	void showFA();
};

class DFA: public FA
{
protected:
	vector<int> endNode;
public:
	DFA(string reg);
	bool checkStr(string& in,int& sym_idx,int& err_t);

};

