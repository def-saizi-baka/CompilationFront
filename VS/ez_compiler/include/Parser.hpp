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
#include "parserTree.h"
#include "Exception.h"
#include "intercode.h"
using namespace std;

extern config con;

class parser
{
public:
	void analysis(const vector<token>& tokens
		,const map<int,vector<pair<int, int>>>& analysisTable);
	vector<int> find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign,int line);
	vector<int> find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign,int line);
	vector<int> find(const map<int, vector<pair<int, int>>>& action, int status, int sign, bool prime,int line);
	int solveConfilct(vector<int>& next_list,int symbol, bool is_in_bool);
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
 	// �����ǰջ
	if(debug)
    	cout << "\n" << setw(4) << id ;  // ��Լ����

    stack<int> signs_tmp = signs;
    stack<int> signs_tmp2;
    while(!signs_tmp.empty()){
        signs_tmp2.push(signs_tmp.top());
        signs_tmp.pop();
    }

    // State
    string status_info= "Stateջ: ";
    for(auto sta : status){
        status_info+=to_string(sta);
        status_info+=", ";
    }
	if(debug)
    	cout << setw(40) << status_info << "  ";

    // signs
    string stack_info = "Signջ: ";
    while(!signs_tmp2.empty()){
        stack_info+=to_string(signs_tmp2.top());
        signs_tmp2.pop();
        if(!signs_tmp2.empty())
            stack_info+=", ";
    }
	if(debug)
    {
		cout << setw(40) << stack_info << endl; // signsջ
    	cout << "============================================" << endl;
	}
}

int parser::solveConfilct(vector<int>& next_list, int symbol, bool is_in_bool)
{
	int next;
	if (next_list.size() == 1) {
		next = next_list[0];
	}
	// �ƽ���Լ��ͻ���
	else {
		if (next_list.size() != 2) {
			cout << "�ƽ���Լ��ͻ����������" << endl;
			for (auto vec : next_list) {
				cout << vec << " ";
			}
			exit(-1);
		}
		int expression = 0;	// ���ʽ
		int next_state = 0;
		int expr_other = 0;
		if (next_list[0] > 0) {
			expression = next_list[1];
			next_state = next_list[0];
		}
		else if (next_list[1] > 0) {
			expression = next_list[0];
			next_state = next_list[1];
		}
		else
		{
			expression = next_list[0];
			expr_other = next_list[1];
			cout << expression << " " << expr_other << endl;
		}
		// �������ȼ�
		pair<int, int> symbol_pre = con.get_operators_info()[symbol];
		// ������ȼ�
		pair<int, vector<int>> item = con.get_grammar()[-expression];
		//cout << item.first << " -->";
		//for (auto vec : item.second) {
		//	cout << vec << " ";
		//}
		pair<int, int> item_pre = { 0, 0 };	// ������ȼ�
		for (int i = item.second.size() - 1; i >= 0; i--) {
			if (item.second[i] < 1000) {
				item_pre = con.get_operators_info()[item.second[i]];
				break;
			}
		}

		pair<int, vector<int>> item_other;
		pair<int, int> item_pre_other = { 0, 0 };	// ������ȼ�
		if (expr_other != 0)	// ˫��Լ
		{
			item_other = con.get_grammar()[-expr_other];
			for (int i = item_other.second.size() - 1; i >= 0; i--) {
				if (item_other.second[i] < 1000) {
					item_pre = con.get_operators_info()[item_other.second[i]];
					break;
				}
			}
		}

		if (item_pre.first == 0)		// ��䲻�������ȼ����˴�����-1���ᱻ����
		{
			/*cout << item.first << " -->";
			for (auto vec : item.second) {
				cout << vec << " ";
			}*/
			// �ƽ���Լ��ͻת�ƽ�
			if (expression * next_state < 0)
			{
				next = next_state;
				return next;
			}
			else if (expr_other < 0) // ��Լ-��Լ��ͻ
			{
				if (item_pre_other.first == 0)	// ˫���������ȼ�����Լ������
				{
					next = (expression < expr_other) ? expression : expr_other;
					if (is_in_bool && symbol == con.get_symbols()[")"]) {
						if (next == expression)
							next = expr_other;
						else if (next == expr_other)
							next = expression;
					}
					cout << next << endl;
					return next;
				}
				else {	// ����һ������ʽ�����ȼ�
					next = expression;
					return next;
				}
			}
		}
		else if (symbol_pre.first < item_pre.first) {	// ����������ȼ���
			next = next_state;
		}
		else if (symbol_pre.first > item_pre.first) {	// ����������ȼ���
			next = expression;
		}
		else
		{
			if (symbol_pre.second == FROM_LEFT_TO_RIGHT) {
				next = expression;
			}
			else
			{
				next = next_state;
			}
		}
	}
	return next;
}

/// <summary>
/// ���س����ƽ���Լ
/// </summary>
/// <param name="tokens"></param> ��״̬Ψһ��ʶ����������Ϊ����
/// <param name="action"></param> action��
/// <param name="_goto"></param> goto��Ϊ�˱���goto�ؼ��ֳ�ͻ
/// <returns></returns> ������ֵ���ã���һ�������õ���״̬����
void parser::analysis(const vector<token>& tokens, const map<int, vector<pair<int, int>>>& analysisTable)
{
	InterCode interCode(this->tree);
	stack<int> signs;
	vector<int> status;
	signs.push(Config::end_int);
	status.push_back(0);//�ӵ�0��״̬��ʼ
	bool is_in_bool = false;
	int sema = 0;
    int _ = 0;
	if(this->debug)
    	cout << "\n" << setiosflags(ios::left) << "��������: " << endl;

	for (unsigned idx = 0; idx < tokens.size();) {
        // debug
        analysis_info(_, status, signs, this->debug);
		if (tokens[idx].symbol == con.get_symbols()["if"] || tokens[idx].symbol == con.get_symbols()["while"])
			is_in_bool = true;
		else if (is_in_bool && tokens[idx].symbol == con.get_symbols()["("])
			sema++;
		else if (is_in_bool && tokens[idx].symbol == con.get_symbols()[")"]) {
			sema--;
			if (sema == 0)
				is_in_bool = false;
		}
		vector<int> next_list = find(analysisTable, status.back(), tokens[idx].symbol, true, tokens[idx].line);
		int next;
		next = solveConfilct(next_list, tokens[idx].symbol, is_in_bool);
		if (next != parser_config::ERROR && next != 0){ 

			// while��֤�ܹ�Լһֱ��Լ
			while (true){
				/* code */	
				next_list = find(analysisTable, status.back(), tokens[idx].symbol, true, tokens[idx].line);
				next = solveConfilct(next_list, tokens[idx].symbol, is_in_bool);
				
				// �ƽ���Լ�ж�
				if (next < 0 && next != parser_config::ERROR) {//action������С��0������Ҫ��Լ
					/* code */	
					int temp = abs(next);//�ҵ�Ҫ��Լ����״̬
					int size = con.get_grammar()[temp].second.size();//�ҵ���Ҫ��ջ���ַ�����Ŀ
                    analysis_info(_, status, signs, this->debug);
					if (this->debug)
						cout << "\t\t��Լ: Ҫ��Լ�����: " << temp << ",  ��ջ����: " << size << endl;

                    pair<int, std::vector<int>> gram = con.get_grammar()[temp];

					if(this->debug)
                    {
						cout << "\t\t��Լ���: " << gram.first << " -> ";
						for(auto out : gram.second){
							cout << out << " ";
						}
						cout << endl;
					}

					for (int j = size - 1; j >= 0; j--) {
						int t_sign = signs.top();
						int t_status = status.back();
						if (con.get_grammar()[temp].second[j] != t_sign) {//���־��Ǵ�����
							string message = "[ERROR] ��Լ���������������룬��ǰ�ķ�����ʽ��ǰ\
									���õ��ķ���Ψһ��ʶ���ǣ�";
							message += to_string(con.get_grammar()[temp].second[j]);
							message += "����������ķ��ŵ�Ψһ��ʶ���ǣ�" + to_string(t_sign);
							con.log(message);
							throw parserException("sign " + con.get__symbols()[t_sign] +" is not allowed here!",tokens[idx].line);
						}
						if(this->debug)
                        	cout << "\t\t\t״̬ "<<t_status<<", ���� "<<t_sign<<" ��ջ"<< endl; 
						signs.pop();
						status.pop_back();
                        analysis_info(_, status, signs, this->debug);
					}
					signs.push(con.get_grammar()[temp].first); //�����ǹ�Լ�õ�����ѹջ
					next_list = find(analysisTable, status.back(), signs.top(), false,tokens[idx].line);
					if(next_list.size() > 1)
						cout <<"  " << next_list.size() << endl;
					status.push_back(next_list[0]);
					tree.reduction(con.get_grammar()[temp]);//��Լ�﷨��
					interCode.genCode(*tree.roots.back(),tokens[idx].line);
				}
				else
					break;
			}

			if (next > 0 && next != parser_config::ACCEPT) {//action���������0������Ҫ�ƽ�
                if(this->debug)
					cout << "\t\t�ƽ�: Status: " << next << " ��ջ, signs: " <<tokens[idx].symbol<<" ��ջ"<<endl;

				status.push_back(next);
				signs.push(tokens[idx].symbol);//������ջ
                analysis_info(_, status, signs, this->debug);
				tree.in(tokens[idx].value, tokens[idx].symbol);
				idx++;//��������һ��״̬

			}
			else if(next == parser_config::ACCEPT){
				this->tree.end();//����������
				con.log("[INFO] �ƽ���Լ�ɹ����");

				interCode.outputCode();
				con.log("[INFO] �м�����������");
				return;
			}
		}
		else{
			throw parserException("sign " + con.get__symbols()[idx] +" is not allowed here!", tokens[idx].line);
			con.log("[ERROR] �ƽ�ʧ�ܣ���ǰ�������ķ���Ψһ��ʶ��Ϊ��" + to_string(idx));
			return;
		}

        _++;
	}
}

/// <summary>
/// ����action��
/// </summary>
/// <param name="action"></param> action��
/// <param name="status"></param> ��ǰ״̬
/// <param name="sign"></param> ǰ���ķ���
/// <returns></returns> ��ȥ����״̬��
vector<int> parser::find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign,int line)
{
	vector<int>res;
	auto iter = action.find(status);
	if (iter == action.end()) {
		con.log("[ERROR] ��ǰ״̬��action�����޷��ҵ�����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
		return vector<int>{parser_config::ERROR};
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			if (this->debug) {
				cout << "\tStatus: " << status << ",  symbol: " << item.first << "  ,action: " << item.second << endl;
			}
			con.log("[INFO] �ɹ���action�����ҵ�״̬ת����Ϣ����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
			res.push_back(item.second);
		}
	}
	if (res.size()) return res;

	throw parserException("sign " + con.get__symbols()[sign] +" is not allowed here!", line);
	con.log("[ERROR] ��ǰ״̬��action����û�н��ܵ�ǰǰ�����ŵı����ǰǰ������Ψһ��ʶ��Ϊ"
		+ to_string(sign));
	return vector<int>{parser_config::ERROR};
}

vector<int> parser::find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign,int line)
{
	vector<int>res;
	auto iter = _goto.find(status);
	if (iter == _goto.end()) {
		con.log("[ERROR] ��ǰ״̬��goto�����޷��ҵ�����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
		return vector<int>{parser_config::ERROR};
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			if(this->debug)
            	cout <<"\tStatus: " << status << ",  symbol: " << item.first <<"  ,goto: "<< item.second << endl;
			con.log("[INFO] �ɹ���goto�����ҵ�״̬ת����Ϣ����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
			res.push_back(item.second);
		}
	}
	if (res.size()) return res;

	throw parserException("sign " + con.get__symbols()[sign] +" is not allowed here!", line);
	con.log("[ERROR] ��ǰ״̬��goto����û�н��ܵ�ǰǰ�����ŵı����ǰǰ������Ψһ��ʶ��Ϊ"
		+ to_string(sign));
	return vector<int>{parser_config::ERROR};
}

vector<int> parser::find(const map<int, vector<pair<int, int>>>& action, int status, int sign, bool prime,int line){
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