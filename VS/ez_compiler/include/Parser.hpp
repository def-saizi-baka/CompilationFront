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

    int _ = 0;
	if(this->debug)
    	cout << "\n" << setiosflags(ios::left) << "��������: " << endl;

	for (unsigned idx = 0; idx < tokens.size();) {
        // debug
        analysis_info(_, status, signs, this->debug);

		int next = find(analysisTable, status.back(), tokens[idx].symbol, true, tokens[idx].line);

		if (next != parser_config::ERROR && next != 0){ 

			while (true){
				/* code */	
				next = find(analysisTable, status.back(), tokens[idx].symbol, true,tokens[idx].line);
				if (next < 0 && next != parser_config::ERROR) {//action������С��0������Ҫ��Լ
					/* code */	
					int temp = abs(next);//�ҵ�Ҫ��Լ����״̬
					int size = con.get_grammar()[temp].second.size();//�ҵ���Ҫ��ջ���ַ�����Ŀ
                    analysis_info(_, status, signs, this->debug);
					if(this->debug)
                    	cout << "\t\t��Լ: Ҫ��Լ�����: "<<temp<<",  ��ջ����: "<< size << endl; 

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
					next = find(analysisTable, status.back(), signs.top(), false,tokens[idx].line);
					status.push_back(next);
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
int parser::find_action(const map<int, vector<pair<int, int>>>& action, int status, int sign,int line)
{
	auto iter = action.find(status);
	if (iter == action.end()) {
		con.log("[ERROR] ��ǰ״̬��action�����޷��ҵ�����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			if(this->debug)
            	cout <<"\tStatus: " << status << ",  symbol: " << item.first <<"  ,action: "<< item.second << endl;
			con.log("[INFO] �ɹ���action�����ҵ�״̬ת����Ϣ����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
			return item.second;
		}
	}
	throw parserException("sign " + con.get__symbols()[sign] +" is not allowed here!", line);
	con.log("[ERROR] ��ǰ״̬��action����û�н��ܵ�ǰǰ�����ŵı����ǰǰ������Ψһ��ʶ��Ϊ"
		+ to_string(sign));
	return parser_config::ERROR;
}

int parser::find_goto(const map<int, vector<pair<int, int>>>& _goto, int status, int sign,int line)
{
	auto iter = _goto.find(status);
	if (iter == _goto.end()) {
		con.log("[ERROR] ��ǰ״̬��goto�����޷��ҵ�����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
		return parser_config::ERROR;
	}
	for (const auto& item : (iter->second)) {
		if (item.first == sign) {
			if(this->debug)
            	cout <<"\tStatus: " << status << ",  symbol: " << item.first <<"  ,goto: "<< item.second << endl;
			con.log("[INFO] �ɹ���goto�����ҵ�״̬ת����Ϣ����ǰ״̬Ψһ��ʶ��Ϊ��" + to_string(status));
			return item.second;
		}
	}
	throw parserException("sign " + con.get__symbols()[sign] +" is not allowed here!", line);
	con.log("[ERROR] ��ǰ״̬��goto����û�н��ܵ�ǰǰ�����ŵı����ǰǰ������Ψһ��ʶ��Ϊ"
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