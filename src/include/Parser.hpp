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
#include "parserTree.hpp"
using namespace std;

extern config con;



class parser
{
public:
	vector<int>&& analysis(const vector<int>& token
		,const map<int,vector<pair<int, int>>>& analysisTable);
	int find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign);
	int find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign);
    int find(const map<int, vector<pair<int, int>>>& action, int status, int sign, bool prime);
	parserTree& get_tree() { return tree; };
private:
	parserTree tree;
};

/// <summary>
/// 主控程序，移进归约
/// </summary>
/// <param name="token"></param> 以状态唯一标识符的序列作为输入
/// <param name="action"></param> action表
/// <param name="_goto"></param> goto表，为了避免goto关键字冲突
/// <returns></returns> 返回右值引用，是一个分析得到的状态序列
vector<int>&& parser::analysis(const vector<int>& token, const map<int, vector<pair<int, int>>>& analysisTable)
{
	stack<int> signs;
	vector<int> status;
	signs.push(Config::end_int);
	status.push_back(0);//从第0个状态开始
	for (unsigned idx = 0; idx < token.size();) {
		int next = find(analysisTable, status.back(), token[idx], true);
		if (next == parser_config::ERROR) {
			con.log("[ERROR] 移进失败，当前分析到的符号唯一标识符为：" + to_string(idx));
			return move(status);
		}
		else if (next > 0 && next != parser_config::ACCEPT) {//action表里面大于0代表需要移进
			status.push_back(next);
			signs.push(token[idx]);//符号入栈
			tree.in(token[idx]);
			idx++;//继续读下一个状态
		}
		else if (next < 0) {//action表里面小于0代表需要归约
			int temp = abs(next);//找到要归约到的状态
			int size = con.get_grammar()[temp].second.size();//找到需要出栈的字符的数目
			for (int j = size - 1; j >= 0; j--) {
				int t_sign = signs.top();
				int t_status = status.back();
				if (con.get_grammar()[temp].second[j] != t_sign) {//这种就是错误发生
					string message = "[ERROR] 归约产生错误，请检查输入，当前文法生成式中前\
							看得到的符号唯一标识号是：";
					message += to_string(con.get_grammar()[temp].second[j]);
					message += "，但是输入的符号的唯一标识号是：" + to_string(t_sign);
					con.log(message);
				}
				signs.pop();
				status.pop_back();
			}
			signs.push(con.get_grammar()[temp].first); //首先是归约得到符号压栈
			next = find(analysisTable, status.back(), signs.top(), false);
			status.push_back(next);
			tree.reduction(con.get_grammar()[temp]);//归约语法树
		}
		else if (next == parser_config::ACCEPT) {
			con.log("[INFO] 成功完成输入串的移进归约分析");
			return move(status);
		}
		else {
			con.log("[ERROR] 移进归约分析错误，请检查action表和goto表");
			return move(status);
		}
	}
	return move(status);
}

/// <summary>
/// 查找action表
/// </summary>
/// <param name="action"></param> action表
/// <param name="status"></param> 当前状态
/// <param name="sign"></param> 前看的符号
/// <returns></returns> 该去往的状态号
int parser::find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign)
{
	auto iter = action.find(status);
	if (iter == action.end()) {
		con.log("[ERROR] 当前状态在action表中无法找到，当前状态唯一标识符为：" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			con.log("[INFO] 成功在action表中找到状态转移信息，当前状态唯一标识符为：" + to_string(status));
			return item.second;
		}
	}
	con.log("[ERROR] 当前状态下action表中没有接受当前前看符号的表项，当前前看符号唯一标识符为"
		+ to_string(sign));
	return parser_config::ERROR;
}

int parser::find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign)
{
	auto iter = _goto.find(status);
	if (iter == _goto.end()) {
		con.log("[ERROR] 当前状态在goto表中无法找到，当前状态唯一标识符为：" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			con.log("[INFO] 成功在goto表中找到状态转移信息，当前状态唯一标识符为：" + to_string(status));
			return item.second;
		}
	}
	con.log("[ERROR] 当前状态下goto表中没有接受当前前看符号的表项，当前前看符号唯一标识符为"
		+ to_string(sign));
	return parser_config::ERROR;
}

int parser::find(const map<int, vector<pair<int, int>>>& action, int status, int sign, bool prime){
    if(prime){
        return find_action(action, status, sign);
    }
    else{
        return find_goto(action, status, sign);
    }
}
