#pragma once
#include <string>
#include <map>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

namespace Config {
	const string path_keyword = "./keywords.txt";
	const string path_operator = "./operator_symbol.txt";
	const string path_delimiter = "./delimiter.txt";
	const string path_unstop = "./unstop.txt";
	const string log_path = "./parser.log";
	const string grammar_path = "./grammar.txt";
	const string end = "$";
	const int end_int = -2;
};

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


inline config::config()
{
	ifstream fin;
	stringstream ssm;
	string temp;
	string content;
	int id;

	fin.open(Config::path_keyword, ios::in);
	bool debug = fin.is_open();
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.keywords[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
	}
	fin.close();

	fin.open(Config::path_delimiter, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.delimiters[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
	}
	fin.close();

	fin.open(Config::path_operator, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.operators[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
	}
	fin.close();

	fin.open(Config::path_unstop, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.unstop_symbols[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
	}
	fin.close();
	this->dic_symbols.symbols[this->dic_symbols.end] = Config::end_int; //加上栈底符号
	this->dic_symbols._symbols[Config::end_int] = this->dic_symbols.end;

	logFile.open(Config::log_path, ios::out);
	assert(logFile.is_open());

	fin.open(Config::grammar_path, ios::in);
	assert(fin.is_open());
	while (fin.peek()!= EOF){
		getline(fin, temp);
		auto pos = temp.find("→");
		if (pos == string::npos) {
			log("[ERROR] 语法表达式错误，缺少→");
			return;
		}
		//切分字符串
		string src = temp.substr(0, pos); //起始状态
		string des = temp.substr(pos + 2); //到达状态
		vector<int> src_list, des_list;
		get_phases_list(src_list, *this, src);
		get_phases_list(des_list, *this, des);
		if (src_list.size() != 1) 
			log("[ERROR] 起始状态集合应该有且只能有1个");
		this->grammar.push_back(make_pair(src_list[0], des_list));
		log(string("[INFO] 成功导入语法表达式" + temp));
	}
	fin.close();
	
}

inline config::~config()
{
	if (logFile.is_open()) {
		log("[END] delete config class");
		logFile.close();
	}
}

void config::log(string str)
{
	this->logFile << str << endl;
}

const map<string, int>& config::get_stop_symbols()const
{
	return this->dic_symbols.stop_symbols;
}

const map<string, int>& config::get_unstop_symbols()const
{
	return this->dic_symbols.unstop_symbols;
	// TODO: 在此处插入 return 语句
}

const map<string, int>& config::get_keywords()const
{
	return this->dic_symbols.keywords;
	// TODO: 在此处插入 return 语句
}

const map<string, int>& config::get_operators()const
{
	return this->dic_symbols.operators;
	// TODO: 在此处插入 return 语句
}

const map<string, int>& config::get_delimiters()const
{
	return this->dic_symbols.delimiters;
	// TODO: 在此处插入 return 语句
}

