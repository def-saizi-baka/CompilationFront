#include <vector>
#include <string>
#include <map>
#include <stack>
using namespace std;


#define Epsilon_CH		'\0'
#define NO_Trans		1
#define READ_FILE_BY_LINE	1
#define READ_REGNEX			2
#define READ_STRING			3
#define KEEP_END		true


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
	void initOP();
	void mergerFA(stack<FA>& tmp, char op);
	FA(char ch);

	void readRegex(string& regex);
	void readStr(string& s);
	void readFile(string& filename);
public:
	FA();
	FA(string& s,int type = READ_REGNEX);
	//FA(string& filename,int type);
	
	void mergeFAbyOr(FA& tmp,bool keepEnd = false);
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

