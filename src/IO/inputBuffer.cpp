#include "inputBuffer.h"

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
		cerr << "未能打开文件" << path << endl;
		return;
	}
	this->isExegesis = false;
	this->line = 0;
}

InputState InputBuffer::readline()
{
	string sline;
	if (fin.peek() != EOF)
		getline(fin, sline);//注意getline读取后自然没有换行符了
	else { //注意到截止位置还读入的话，要给予一个错误
		if (this->isExegesis == false)
			return InputState::END_OF_FILE;
		else
			return InputState::NO_CLOSED_COMMENT;//未闭合的注释
	}
	line++; //找错误用的，假如要报错误提示，这个变量可以显示错误在哪一行
	//接下来处理注释
	string::size_type idx1, idx2, idx3, _idx1, _idx2;
	InputState res = InputState::CORRECT;
	while (true) {
		idx1 = sline.find("//");
		idx2 = sline.find("/*");
		idx3 = sline.find("*/");
		_idx1 = sline.find("\"");
		_idx2 = sline.find("\"", _idx1 + 1);//找第二个
		if (_idx1 != string::npos && _idx2 != string::npos) { //存在双引号，需要就此做一些修正
			if (_idx1 < idx1 && idx1 < _idx2)
				idx1 = string::npos;
			if (_idx1 < idx2 && idx2 < _idx2)
				idx2 = string::npos;
			if (_idx1 < idx3 && idx3 < _idx2)
				idx3 = string::npos;
		}

		if (this->isExegesis == true && idx3 == string::npos) //注释态没读到 */
			return res;
		if (this->isExegesis == false && idx1 == string::npos && idx2 == string::npos && idx3 == string::npos)
			//非注释态都没读到 
			break;

		if (this->isExegesis == true) {//在注释态
			if (idx3 == string::npos)  //没找到对应的 */,直接跳过这一行
				break;
			else { //找到了
				sline = sline.substr(idx3 + 2);
				this->isExegesis = false;//进入非注释态
			}
		}
		else {
			string::size_type m = ::min(idx1, idx2, idx3);
			//未在注释态

			if (m == idx1) { //  //在第一位，后面的全部不用了
				sline = sline.substr(0, idx1);
				break;
			}
			else if (m == idx2) { //  /* 在第一位
				this->isExegesis = true;//直接进入注释态
				buffer += sline.substr(0, idx2);//前面的先入缓冲区
				sline = sline.substr(idx2 + 2);//到底在放到新的字符串里面
			}
			else if (m == idx3) { // */ 在第一位，这是错误的
				//这个处理待会看
				res = InputState::ERROR_COMMENT;//错误的注释
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
		temp = readline(); //读一行
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
	this->fin.seekg(0, ios::beg);//回起始位置
}

bool InputBuffer::open(const string& path)
{
	this->path = path;
	fin.open(path, ios::in);
	if (!fin.is_open()) {
		cerr << "未能打开文件" << path << endl;
		return false;
	}
	return true;
}

const string& InputBuffer::getBuffer() const
{
	return this->buffer;// TODO: 在此处插入 return 语句
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
