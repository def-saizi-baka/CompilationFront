#pragma once
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <cassert>
#include "config.hpp"
#include <stack>
#include <cmath>
#include "parserTree.hpp"
using namespace std;

extern config con;

namespace parser_config {
	const int ACCEPT = 10000;
	const int ERROR = -10000;
};

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
/// ���س����ƽ���Լ
/// </summary>
/// <param name="token"></param> ��״̬Ψһ��ʶ����������Ϊ����
/// <param name="action"></param> action��
/// <param name="_goto"></param> goto��Ϊ�˱���goto�ؼ��ֳ�ͻ
/// <returns></returns> ������ֵ���ã���һ�������õ���״̬����
vector<int>&& parser::analysis(const vector<int>& token, const map<int, vector<pair<int, int>>>& analysisTable)
{
	stack<int> signs;
	vector<int> status;
	signs.push(Config::end_int);
	status.push_back(0);//�ӵ�0��״̬��ʼ
	for (unsigned idx = 0; idx < token.size();) {
		int next = find(analysisTable, status.back(), token[idx], true);
		if (next == parser_config::ERROR) {
			con.log("[ERROR] �ƽ�ʧ�ܣ���ǰ�������ķ���Ψһ��ʶ��Ϊ��" + to_string(idx));
			return move(status);
		}
		else if (next > 0 && next != parser_config::ACCEPT) {//action���������0������Ҫ�ƽ�
			status.push_back(next);
			signs.push(token[idx]);//������ջ
			tree.in(token[idx]);
			idx++;//��������һ��״̬
		}
		else if (next < 0) {//action������С��0������Ҫ��Լ
			int temp = abs(next);//�ҵ�Ҫ��Լ����״̬
			size_t size = con.get_grammar()[temp].second.size();//�ҵ���Ҫ��ջ���ַ�����Ŀ
			for (size_t j = size - 1; j >= 0; j--) {
				int t_sign = signs.top();
				int t_status = status.back();
				if (con.get_grammar()[temp].second[j] != t_sign) {//���־��Ǵ�����
					string message = "[ERROR] ��Լ���������������룬��ǰ�ķ�����ʽ��ǰ\
							���õ��ķ���Ψһ��ʶ���ǣ�";
					message += to_string(con.get_grammar()[temp].second[j]);
					message += "����������ķ��ŵ�Ψһ��ʶ���ǣ�" + to_string(t_sign);
					con.log(message);
				}
				signs.pop();
				status.pop_back();
			}
			signs.push(con.get_grammar()[temp].first); //�����ǹ�Լ�õ�����ѹջ
			next = find(analysisTable, status.back(), signs.top(), false);
			status.push_back(next);
			tree.reduction(con.get_grammar()[temp]);//��Լ�﷨��
		}
		else if (next == parser_config::ACCEPT) {
			con.log("[INFO] �ɹ�������봮���ƽ���Լ����");
			return move(status);
		}
		else {
			con.log("[ERROR] �ƽ���Լ������������action���goto��");
			return move(status);
		}
	}
	return move(status);
}

/// <summary>
/// ����action��
/// </summary>
/// <param name="action"></param> action��
/// <param name="status"></param> ��ǰ״̬
/// <param name="sign"></param> ǰ���ķ���
/// <returns></returns> ��ȥ����״̬��
int parser::find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign)
{
	auto iter = action.find(status);
	if (iter == action.end()) {
		con.log("[ERROR] ��ǰ״̬��action�����޷��ҵ�����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == status) {
			con.log("[INFO] �ɹ���action�����ҵ�״̬ת����Ϣ����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
			return item.second;
		}
	}
	con.log("[ERROR] ��ǰ״̬��action����û�н��ܵ�ǰǰ�����ŵı����ǰǰ������Ψһ��ʶ��Ϊ"
		+ to_string(sign));
	return parser_config::ERROR;
}

int parser::find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign)
{
	auto iter = _goto.find(status);
	if (iter == _goto.end()) {
		con.log("[ERROR] ��ǰ״̬��goto�����޷��ҵ�����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == status) {
			con.log("[INFO] �ɹ���goto�����ҵ�״̬ת����Ϣ����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
			return item.second;
		}
	}
	con.log("[ERROR] ��ǰ״̬��goto����û�н��ܵ�ǰǰ�����ŵı����ǰǰ������Ψһ��ʶ��Ϊ"
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
