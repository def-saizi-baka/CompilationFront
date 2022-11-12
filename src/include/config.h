#pragma once
#include <string>
#include <map>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "const.h"
using namespace std;



typedef struct{
	string regex;
	int endType = -1;
	bool raw = false;
} regex_exp;

class config
{
public:
	config();
	~config();
	void log(string str);
	const map<string, int>& get_stop_symbols()const;
	const map<string, int>& get_unstop_symbols()const;
	const map<string, int>& get_keywords()const;
	const map<string, int>& get_operators()const;
	const map<string, int>& get_delimiters()const;
	const vector<regex_exp>& get_regex() const;
	const vector<pair<int, vector<int>>>& get_grammar()const { return this->grammar; };
	const map<string, int>& get_symbols() const { return this->dic_symbols.symbols; };
	map<int, string>& get__symbols(){ return this->dic_symbols._symbols; };
private:
	struct {
		map<string, int> stop_symbols;
		map<string, int> unstop_symbols;
		map<string, int> keywords;
		map<string, int> operators;
		map<string, int> delimiters;
		map<string, int> symbols;
		map<int, string> _symbols;
		string end = Config::end; //用作归约的终止符
	}dic_symbols; //符号字典，把每一个符号映射到一个int值 
	vector<pair<int, vector<int>>> grammar;//所有文法表
	ofstream logFile;
	vector<regex_exp> regexList;
};

static void get_phases_list(vector<int>& res, config& con, string input)
{
	size_t i, j;
	bool flag = false;
	for (i = j = 0; j < input.size(); j++) {
		if (input[j] == '<' && flag == false) {
			i = j;
			flag = true;
		}
		else if (input[j] == '>' && flag == true) {
			flag = false;
			if (j < i + 2)
				continue;
			else {
				string temp = input.substr(i + 1, j - i - 1);//得到状态名
				temp.erase(0, temp.find_first_not_of(" "));
				temp.erase(temp.find_last_not_of(" ") + 1);
				//以上这些操作是避免去除空格
				auto iter_stop = con.get_stop_symbols().find(temp);
				if (iter_stop != con.get_stop_symbols().end())
					res.push_back(iter_stop->second);
				else {
					auto iter_unstop = con.get_unstop_symbols().find(temp);
					if (iter_unstop != con.get_unstop_symbols().end())
						res.push_back(iter_unstop->second);
					else {
						con.log(string("[ERROR] 没有这个状态 ") + temp);
					}
				}
			}
		}
	}
};

