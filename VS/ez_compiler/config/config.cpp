#include "../include/config.h"

config::config()
{
	;
}

void config::init()
{
	ifstream fin;
	stringstream ssm;
	string temp;
	string content;
	int raw;
	string regex;
	int id;

	fin.open(path_keyword, ios::in);
	bool debug = fin.is_open();
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
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

	fin.open(path_delimiter, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		ssm >> raw;
		ssm >> regex;
		if (raw == 0)
			ssm >> regex;
		else
			regex = content;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.delimiters[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
		this->regexList.push_back(regex_exp{ regex,id,raw == 1 });
	}
	fin.close();

	fin.open(path_operator, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		int priority, binding;
		ssm >> priority;
		ssm >> binding;
		ssm >> raw;
		ssm >> regex;
		if (raw == 0)
			ssm >> regex;
		else
			regex = content;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.operators[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
		this->dic_symbols.operators_info[id] = make_pair(priority, binding);
		this->regexList.push_back(regex_exp{ regex,id,raw == 1 });
	}
	fin.close();

	fin.open(path_unstop, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.unstop_symbols[content] = id;
		this->dic_symbols.symbols[content] = id;
		this->dic_symbols._symbols[id] = content;
	}
	fin.close();
	this->dic_symbols.symbols[this->dic_symbols.end] = Config::end_int; //����ջ�׷���
	this->dic_symbols._symbols[Config::end_int] = this->dic_symbols.end;

	logFile.open(log_path, ios::out);
	assert(logFile.is_open());

	fin.open(grammar_path, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		auto pos = temp.find("��");
		if (pos == string::npos) {
			log("[ERROR] �﷨���ʽ����ȱ�١�");
			return;
		}
		//�з��ַ���
		string src = temp.substr(0, pos); //��ʼ״̬
		string des = temp.substr(pos + 2); //����״̬
		vector<int> src_list, des_list;
		get_phases_list(src_list, *this, src);
		get_phases_list(des_list, *this, des);
		if (src_list.size() != 1)
			log("[ERROR] ��ʼ״̬����Ӧ������ֻ����1��");
		this->grammar.push_back(make_pair(src_list[0], des_list));
		log(string("[INFO] �ɹ������﷨���ʽ" + temp));
	}
	fin.close();
}

string config::get_name(int symbol)
{
	return dic_symbols._symbols[symbol];
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
	// TODO: �ڴ˴����� return ���
}

const map<string, int>& config::get_keywords()const
{
	return this->dic_symbols.keywords;
	// TODO: �ڴ˴����� return ���
}

const map<string, int>& config::get_operators()const
{
	return this->dic_symbols.operators;
	// TODO: �ڴ˴����� return ���
}

const map<string, int>& config::get_delimiters()const
{
	return this->dic_symbols.delimiters;
	// TODO: �ڴ˴����� return ���
}

const vector<regex_exp>& config::get_regex() const
{
	return this->regexList;
	// TODO: �ڴ˴����� return ���
}

map<int, pair<int, int>> config::get_operators_info()
{
	return this->dic_symbols.operators_info;
}

