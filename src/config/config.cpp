#include "config.h"

config::config()
{
	ifstream fin;
	stringstream ssm;
	string temp;
	string content;
	int raw;
	string regex;
	int id;

	fin.open(Config::path_keyword, ios::in);
	bool debug = fin.is_open();
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm >> raw;
		if (raw == 0)
			ssm >> regex;
		else
			regex = content;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.keywords[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
		this->regexList.push_back(regex_exp{ regex,id,raw == 1 });
	}
	fin.close();

	fin.open(Config::path_delimiter, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm >> raw;
		ssm >> regex;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.delimiters[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
		this->regexList.push_back(regex_exp{ regex,id,raw == 1 });
	}
	fin.close();

	fin.open(Config::path_operator, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm >> raw;
		ssm >> regex;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.operators[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
		this->regexList.push_back(regex_exp{ regex,id,raw == 1 });
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


config::~config()
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

const vector<regex_exp>& config::get_regex() const
{
	return this->regexList;
	// TODO: 在此处插入 return 语句
}

