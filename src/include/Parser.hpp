#pragma once
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <cassert>
#include "config.h"
#include <stack>
#include <cmath>
#include <iomanip>
#include "parserTree.hpp"
#include "Exception.h"
using namespace std;

extern config con;

class parser
{
public:
	void analysis(const vector<token>& tokens
		,const map<int,vector<pair<int, int>>>& analysisTable);
	int find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign,int line);
	int find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign,int line);
    int find(const map<int, vector<pair<int, int>>>& action, int status, int sign, bool prime,int line);
	void setDebug(bool debug);
	parserTree& get_tree() { return tree; };
private:
	parserTree tree;
	bool debug;
};

void parser::setDebug(bool debug)
{
	this->debug = debug;
}

void analysis_info(int id,const vector<int>& status,const stack<int>& signs,bool debug){
 	// 输出当前栈
	if(debug)
    	cout << "\n" << setw(4) << id ;  // 规约步骤

    stack<int> signs_tmp = signs;
    stack<int> signs_tmp2;
    while(!signs_tmp.empty()){
        signs_tmp2.push(signs_tmp.top());
        signs_tmp.pop();
    }

    // State
    string status_info= "State栈: ";
    for(auto sta : status){
        status_info+=to_string(sta);
        status_info+=", ";
    }
	if(debug)
    	cout << setw(40) << status_info << "  ";

    // signs
    string stack_info = "Sign栈: ";
    while(!signs_tmp2.empty()){
        stack_info+=to_string(signs_tmp2.top());
        signs_tmp2.pop();
        if(!signs_tmp2.empty())
            stack_info+=", ";
    }
	if(debug)
    {
		cout << setw(40) << stack_info << endl; // signs栈
    	cout << "============================================" << endl;
	}
}

/// <summary>
/// 主控程序，移进归约
/// </summary>
/// <param name="tokens"></param> 以状态唯一标识符的序列作为输入
/// <param name="action"></param> action表
/// <param name="_goto"></param> goto表，为了避免goto关键字冲突
/// <returns></returns> 返回右值引用，是一个分析得到的状态序列
void parser::analysis(const vector<token>& tokens, const map<int, vector<pair<int, int>>>& analysisTable)
{
	stack<int> signs;
	vector<int> status;
	signs.push(Config::end_int);
	status.push_back(0);//从第0个状态开始

    int _ = 0;
	if(this->debug)
    	cout << "\n" << setiosflags(ios::left) << "分析过程: " << endl;

	for (unsigned idx = 0; idx < tokens.size();) {
        // debug
        analysis_info(_, status, signs, this->debug);

		int next = find(analysisTable, status.back(), tokens[idx].symbol, true, tokens[idx].line);

		if (next != parser_config::ERROR && next != 0){ 

			while (true){
				/* code */	
				next = find(analysisTable, status.back(), tokens[idx].symbol, true,tokens[idx].line);
				if (next < 0 && next != parser_config::ERROR) {//action表里面小于0代表需要归约
					/* code */	
					int temp = abs(next);//找到要归约到的状态
					int size = con.get_grammar()[temp].second.size();//找到需要出栈的字符的数目
                    analysis_info(_, status, signs, this->debug);
					if(this->debug)
                    	cout << "\t\t规约: 要规约的语句: "<<temp<<",  出栈个数: "<< size << endl; 

                    pair<int, std::vector<int>> gram = con.get_grammar()[temp];

					if(this->debug)
                    {
						cout << "\t\t规约语句: " << gram.first << " -> ";
						for(auto out : gram.second){
							cout << out << " ";
						}
						cout << endl;
					}

					for (int j = size - 1; j >= 0; j--) {
						int t_sign = signs.top();
						int t_status = status.back();
						if (con.get_grammar()[temp].second[j] != t_sign) {//这种就是错误发生
							string message = "[ERROR] 归约产生错误，请检查输入，当前文法生成式中前\
									看得到的符号唯一标识号是：";
							message += to_string(con.get_grammar()[temp].second[j]);
							message += "，但是输入的符号的唯一标识号是：" + to_string(t_sign);
							con.log(message);
							throw parserException("sign " + con.get__symbols()[t_sign] +" is not allowed here!",tokens[idx].line);
						}
						if(this->debug)
                        	cout << "\t\t\t状态 "<<t_status<<", 符号 "<<t_sign<<" 出栈"<< endl; 
						signs.pop();
						status.pop_back();
                        analysis_info(_, status, signs, this->debug);
					}
					signs.push(con.get_grammar()[temp].first); //首先是归约得到符号压栈
					next = find(analysisTable, status.back(), signs.top(), false,tokens[idx].line);
					status.push_back(next);
					tree.reduction(con.get_grammar()[temp]);//归约语法树
				}
				else
					break;
			}

			if (next > 0 && next != parser_config::ACCEPT) {//action表里面大于0代表需要移进
                if(this->debug)
					cout << "\t\t移进: Status: " << next << " 入栈, signs: " <<tokens[idx].symbol<<" 入栈"<<endl;

				status.push_back(next);
				signs.push(tokens[idx].symbol);//符号入栈
                analysis_info(_, status, signs, this->debug);
				tree.in(tokens[idx].symbol);
				idx++;//继续读下一个状态

			}
			else if(next == parser_config::ACCEPT){
				this->tree.end();//结束，建树
				con.log("[INFO] 移进归约成功完成");
				return;
			}
		}
		else{
			throw parserException("sign " + con.get__symbols()[idx] +" is not allowed here!", tokens[idx].line);
			con.log("[ERROR] 移进失败，当前分析到的符号唯一标识符为：" + to_string(idx));
			return;
		}

        _++;
	}
}

/// <summary>
/// 查找action表
/// </summary>
/// <param name="action"></param> action表
/// <param name="status"></param> 当前状态
/// <param name="sign"></param> 前看的符号
/// <returns></returns> 该去往的状态号
int parser::find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign,int line)
{
	auto iter = action.find(status);
	if (iter == action.end()) {
		con.log("[ERROR] 当前状态在action表中无法找到，当前状态唯一标识符为：" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			if(this->debug)
            	cout <<"\tStatus: " << status << ",  symbol: " << item.first <<"  ,action: "<< item.second << endl;
			con.log("[INFO] 成功在action表中找到状态转移信息，当前状态唯一标识符为：" + to_string(status));
			return item.second;
		}
	}
	throw parserException("sign " + con.get__symbols()[sign] +" is not allowed here!", line);
	con.log("[ERROR] 当前状态下action表中没有接受当前前看符号的表项，当前前看符号唯一标识符为"
		+ to_string(sign));
	return parser_config::ERROR;
}

int parser::find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign,int line)
{
	auto iter = _goto.find(status);
	if (iter == _goto.end()) {
		con.log("[ERROR] 当前状态在goto表中无法找到，当前状态唯一标识符为：" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			if(this->debug)
            	cout <<"\tStatus: " << status << ",  symbol: " << item.first <<"  ,goto: "<< item.second << endl;
			con.log("[INFO] 成功在goto表中找到状态转移信息，当前状态唯一标识符为：" + to_string(status));
			return item.second;
		}
	}
	throw parserException("sign " + con.get__symbols()[sign] +" is not allowed here!", line);
	con.log("[ERROR] 当前状态下goto表中没有接受当前前看符号的表项，当前前看符号唯一标识符为"
		+ to_string(sign));
	return parser_config::ERROR;
}

int parser::find(const map<int, vector<pair<int, int>>>& action, int status, int sign, bool prime,int line){
    // cout << '\t' << setw(15) << "Status: "+to_string(status) <<"  ";
    // cout << '\t' << setw(15) << "Sign: "+to_string(sign)<<"  ";
    // cout << '\t' << setw(15) << "Type: " + string(prime ? "action" : "goto")<<endl;

    if(prime){
        return find_action(action, status, sign, line);
    }
    else{
        return find_goto(action, status, sign,line);
    }
}