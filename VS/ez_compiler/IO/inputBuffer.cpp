#include "../include/inputBuffer.h"

InputBuffer::InputBuffer()
{
	this->isExegesis = false;
	this->line = 0;
}

InputBuffer::InputBuffer(const string& path)
{
	this->path = path;
	fin.open(path, ios::in);
	if (!fin.is_open()) {
		cerr << "δ�ܴ��ļ�" << path << endl;
		return;
	}
	this->isExegesis = false;
	this->line = 0;
}

InputState InputBuffer::readline()
{
	string sline;
	if (fin.peek() != EOF)
		getline(fin, sline);//ע��getline��ȡ����Ȼû�л��з���
	else { //ע�⵽��ֹλ�û�����Ļ���Ҫ����һ������
		if (this->isExegesis == false)
			return InputState::END_OF_FILE;
		else
			return InputState::NO_CLOSED_COMMENT;//δ�պϵ�ע��
	}
	line++; //�Ҵ����õģ�����Ҫ��������ʾ���������������ʾ��������һ��
	//����������ע��
	string::size_type idx1, idx2, idx3, _idx1, _idx2;
	InputState res = InputState::CORRECT;
	while (true) {
		idx1 = sline.find("//");
		idx2 = sline.find("/*");
		idx3 = sline.find("*/");
		_idx1 = sline.find("\"");
		_idx2 = sline.find("\"", _idx1 + 1);//�ҵڶ���
		if (_idx1 != string::npos && _idx2 != string::npos) { //����˫���ţ���Ҫ�ʹ���һЩ����
			if (_idx1 < idx1 && idx1 < _idx2)
				idx1 = string::npos;
			if (_idx1 < idx2 && idx2 < _idx2)
				idx2 = string::npos;
			if (_idx1 < idx3 && idx3 < _idx2)
				idx3 = string::npos;
		}

		if (this->isExegesis == true && idx3 == string::npos) //ע��̬û���� */
			return res;
		if (this->isExegesis == false && idx1 == string::npos && idx2 == string::npos && idx3 == string::npos)
			//��ע��̬��û���� 
			break;

		if (this->isExegesis == true) {//��ע��̬
			if (idx3 == string::npos)  //û�ҵ���Ӧ�� */,ֱ��������һ��
				break;
			else { //�ҵ���
				sline = sline.substr(idx3 + 2);
				this->isExegesis = false;//�����ע��̬
			}
		}
		else {
			string::size_type m = ::min(idx1, idx2, idx3);
			//δ��ע��̬

			if (m == idx1) { //  //�ڵ�һλ�������ȫ��������
				sline = sline.substr(0, idx1);
				break;
			}
			else if (m == idx2) { //  /* �ڵ�һλ
				this->isExegesis = true;//ֱ�ӽ���ע��̬
				buffer += sline.substr(0, idx2);//ǰ������뻺����
				sline = sline.substr(idx2 + 2);//�����ڷŵ��µ��ַ�������
			}
			else if (m == idx3) { // */ �ڵ�һλ�����Ǵ����
				//���������ῴ
				res = InputState::ERROR_COMMENT;//�����ע��
				buffer += sline.substr(0, idx3);
				sline = sline.substr(idx3 + 2);
			}
		}
	}
	buffer += sline;
	return res;
}



InputState InputBuffer::readAll()
{
	InputState res = InputState::CORRECT;
	InputState temp;
	while (true){
		temp = readline(); //��һ��
		if (temp == InputState::END_OF_FILE) 
			break;
		else if (temp == InputState::ERROR_COMMENT || temp == InputState::NO_CLOSED_COMMENT)
			res = temp;
	}
	return res;
}

bool InputBuffer::getIsExegesis()
{
	return this->isExegesis;
}

int InputBuffer::getLineNumber()
{
	return line;
}

void InputBuffer::pop(string& s)
{
	s = this->buffer;
	this->buffer.clear();
}

void InputBuffer::reset()
{
	this->line = 0;
	this->buffer.clear();
	this->fin.clear();
	this->fin.seekg(0, ios::beg);//����ʼλ��
}

bool InputBuffer::open(const string& path)
{
	this->path = path;
	fin.open(path, ios::in);
	if (!fin.is_open()) {
		cerr << "δ�ܴ��ļ�" << path << endl;
		return false;
	}
	return true;
}

const string& InputBuffer::getBuffer() const
{
	return this->buffer;// TODO: �ڴ˴����� return ���
}

InputBuffer::~InputBuffer()
{
	if (this->fin.is_open())
		this->fin.close();
}

string::size_type min(string::size_type a, string::size_type b, string::size_type c)
{
	if (a < b) {
		if (a < c)
			return a;
		else
			return c;
	}
	else {
		if (b < c)
			return b;
		else
			return c;
	}
}
