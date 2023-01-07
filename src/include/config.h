#pragma once
#include <string>
#include <map>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "const.h"
#include <iostream>
#define FROM_LEFT_TO_RIGHT 0
#define FROM_RIGHT_TO_LEFT 1
using namespace std;


struct token {
	int line = 0;
	string value = "";
	int symbol = -1;
};

struct regex_exp {
	string regex;
	int endType = -1;
	bool raw = false;
};

class config
{
public:
	config();
	~config();
	void init();
	void log(string str);
	const map<string, int>& get_stop_symbols()const;
	const map<string, int>& get_unstop_symbols()const;
	const map<string, int>& get_keywords()const;
	const map<string, int>& get_operators()const;
	const map<string, int>& get_delimiters()const;
	const vector<regex_exp>& get_regex() const;
	map<int, pair<int, int>> get_operators_info();
	string get_name(int value);
	const vector<pair<int, vector<int>>>& get_grammar()const { return this->grammar; };
	map<string, int>& get_symbols() { return this->dic_symbols.symbols; };
	map<int, string>& get__symbols() { return this->dic_symbols._symbols; };

	string path_keyword = "config/keywords.txt";
	string path_operator = "config/operator_symbol.txt";
	string path_delimiter = "config/delimiter.txt";
	string path_unstop = "config/unstop.txt";
	string log_path = "config/parser.log";
	string grammar_path = "config/grammar.txt";

private:
	struct {
		map<string, int> stop_symbols;
		map<string, int> unstop_symbols;
		map<string, int> keywords;
		map<string, int> operators;
		map<string, int> delimiters;
		map<string, int> symbols;
		map<int, string> _symbols;
		map<int, pair<int, int>> operators_info;
		string end = Config::end;//用作归约的终止符
	}dic_symbols; ///符号字典，把每一个符号映射到一个int值 
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
				size_t t = temp.find("large");
				while(true){
					if(t != string::npos){
						string temp_2 = temp.substr(t + 5);
						string temp_1 = temp.substr(0, t);
						temp = temp_1 + ">" + temp_2;	
					}
					else
						break;
					t = temp.find("large");
				}
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

