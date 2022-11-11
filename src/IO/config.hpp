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

static void get_phases_list(vector<int>& res, config& config, string input)
{
	size_t i, j;
	bool flag = false;
	for (i = j = 0; j < input.size(); j++) {
		if (input[i] == '<' && flag == false) {
			j = i;
			flag = true;
		}
		else if (input[i] == '>' && flag == true) {
			flag = false;
			if (j < i + 2)
				continue;
			else {
				string temp = input.substr(i + 1, j - i - 1);//�õ�״̬��
				stringstream ssm;
				ssm << temp;
				ssm >> temp;
				//������Щ�����Ǳ���ȥ���ո�
				auto iter_stop = config.get_stop_symbols().find(temp);
				if (iter_stop != config.get_stop_symbols().end())
					res.push_back(iter_stop->second);
				else {
					auto iter_unstop = config.get_unstop_symbols().find(temp);
					if (iter_unstop != config.get_unstop_symbols().end())
						res.push_back(iter_unstop->second);
					else {
						config.log(string("[ERROR] û�����״̬ ") + temp);
					}
				}
			}
		}
	}
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
private:
	struct {
		map<string, int> stop_symbols;
		map<string, int> unstop_symbols;
		map<string, int> keywords;
		map<string, int> operators;
		map<string, int> delimiters;
		map<string, int> symbols;
		string end = Config::end; //������Լ����ֹ��
	}dic_symbols; //�����ֵ䣬��ÿһ������ӳ�䵽һ��intֵ 
	vector<pair<int, vector<int>>> grammar;//�����ķ���
	ofstream logFile;
};

inline config::config()
{
	ifstream fin;
	stringstream ssm;
	string temp;
	string content;
	int id;

	fin.open(Config::path_keyword, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.keywords[content] = id;
		this->dic_symbols.symbols[content] = id;
	}
	fin.close();

	fin.open(Config::path_delimiter, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.delimiters[content] = id;
		this->dic_symbols.symbols[content] = id;
	}
	fin.close();

	fin.open(Config::path_operator, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.stop_symbols[content] = id;
		this->dic_symbols.operators[content] = id;
		this->dic_symbols.symbols[content] = id;
	}
	fin.close();

	fin.open(Config::path_unstop, ios::in);
	assert(fin.is_open());
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//ȫ������ȥ
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->dic_symbols.unstop_symbols[content] = id;
		this->dic_symbols.symbols[content] = id;
	}
	fin.close();
	this->dic_symbols.symbols[this->dic_symbols.end] = Config::end_int; //����ջ�׷���

	logFile.open(Config::log_path, ios::out);
	assert(logFile.is_open());

	fin.open(Config::grammar_path, ios::in);
	assert(fin.is_open());
	while (fin.peek()!= EOF){
		getline(fin, temp);
		auto pos = temp.find("��");
		if (pos == string::npos) {
			log("[ERROR] �﷨���ʽ����ȱ�١�");
			return;
		}
		//�з��ַ���
		string src = temp.substr(0, pos); //��ʼ״̬
		string des = temp.substr(pos); //����״̬
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

