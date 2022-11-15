#include "symbolTable.h"

info::info()
{
	this->type = Type::none;
}

info::info(string name, Type type)
{
	this->name = name;
	this->type = type;
}

info::info(const info& i)
{
	this->name = i.name;
	this->type = i.type;
}

void info::print()const
{
	cout << name << "\t" << type << endl;
}

bool symbolTable::insert(const input& t)
{
	if (t.Type == Type::const_number_int) {
		return symbolTables[(int)Type::const_number_int]->insert(t);
	}
	else if (t.Type == Type::const_number_double) {
		return symbolTables[(int)Type::const_number_double]->insert(t);
	}
	else if (t.Type == Type::const_number_char) {
		return symbolTables[(int)Type::const_number_char]->insert(t);
	}
	else if (t.Type == Type::const_number_str) {
		return symbolTables[(int)Type::const_number_str]->insert(t);
	}
	else if (t.Type == Type::identification) {
		return symbolTables[(int)Type::identification]->insert(t);
	}
	else {
		cerr << "所指定的类型无法插入" << endl;
		return false;
		//其他东西不是动态的，这里不允许插入
	}
}

void symbolTable::print()
{
	cout << "符号表\n";
	cout << "名称" << "\t" << "类型\n";
	for (const auto& t : this->symbolTables) {
		t.second->print();
		/*
		if (t.second->type == Type::identification) {
			t.second->print();
		}
		else if (t.second->type == Type::const_number_int) {
			t.second->print();
		}
		else if (t.second->type == Type::const_number_double) {
			t.second->print();
		}
		else if (t.second->type == Type::const_number_char) {
			t.second->print();
		}
		else if (t.second->type == Type::const_number_str) {
			t.second->print();
		}
		else {
			t.second->print();
		}*/
	}
}

symbolTable::~symbolTable()
{
	for (auto& t : this->symbolTables) {
		delete t.second;
	}
}

symbolTable::symbolTable()
{
	string temp, content;
	int id;
	stringstream ssm;

	this->symbolTables[(int)Type::identification] = new identificationInfo();
	this->symbolTables[(int)Type::const_number_int] = new intInfo();
	this->symbolTables[(int)Type::const_number_double] = new doubleInfo();
	this->symbolTables[(int)Type::const_number_char] = new charInfo();
	this->symbolTables[(int)Type::const_number_str] = new strInfo();

	ifstream fin;
	fin.open(path_keyword);
	if (!fin.is_open()) {
		cerr << path_keyword << "该路径下没有配置文件\n";
		return;
	}

	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->symbolTables[id] = new info(content, Type::keywords);
	}
	fin.close();

	fin.open(path_operator);
	if (!fin.is_open()) {
		cerr << path_operator << "该路径下没有配置文件\n";
		return;
	}
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->symbolTables[id] = new info(content, Type::operator_symbol);
	}
	fin.close();

	fin.open(path_delimiter);
	if (!fin.is_open()) {
		cerr << path_delimiter << "该路径下没有配置文件\n";
		return;
	}
	while (fin.peek() != EOF) {
		getline(fin, temp);
		ssm << temp;//全部放下去
		ssm >> content;
		ssm >> id;
		ssm.clear();
		this->symbolTables[id] = new info(content, Type::delimiter);
	}
	fin.close();
}

int identificationInfo::hashf(string& name)
{
	int ascii_sum = 0;
	for (const auto& i : name) {
		ascii_sum += i;
	}
	return ascii_sum;
}

void identificationInfo::print()const
{
	cout << "标识符表\n";
	for (const auto& t : this->symbols) {
		cout << t.second.name << "\t" << t.second.type << endl;
	}
}

bool identificationInfo::insert(const input& t)
{
	id i;
	i.name = t.id.name;
	i.type = t.id.type;
	int key = hashf(i.name);
	if (this->symbols.count(key) == 0) {
		this->symbols[key] = i;
		return true;
	}
	else {
		cerr << "插入失败，已存在key=" << key << endl;
		return false;
	}
}

string intInfo::hashf(idInt t)
{
	stringstream ssm;
	if (t.type == "bool") {
		ssm << t.name.nameBool;
	}
	else if (t.type == "int") {
		ssm << t.name.nameInt;
	}
	else if (t.type == "short") {
		ssm << t.name.nameShort;
	}
	else if (t.type == "long") {
		ssm << t.name.nameLong;
	}
	else if (t.type == "long long") {
		ssm << t.name.nameLongLong;
	}
	else if (t.type == "unsigned") {
		ssm << t.name.nameUInt;
	}
	else if (t.type == "unsigned short") {
		ssm << t.name.nameUShort;
	}
	else if (t.type == "unsigned long") {
		ssm << t.name.nameULong;
	}
	else if (t.type == "unsigned long long") {
		ssm << t.name.nameULongLong;
	}
	string res;
	ssm >> res;
	return res;
}

void intInfo::print()const
{
	cout << "整型常数表\n";
	for (const auto& t : this->symbols) {
		if (t.second.type == "bool") {
			cout << t.second.name.nameBool;
		}
		else if (t.second.type == "int") {
			cout << t.second.name.nameInt;
		}
		else if (t.second.type == "short") {
			cout << t.second.name.nameShort;
		}
		else if (t.second.type == "long") {
			cout << t.second.name.nameLong;
		}
		else if (t.second.type == "long long") {
			cout << t.second.name.nameLongLong;
		}
		else if (t.second.type == "unsigned") {
			cout << t.second.name.nameUInt;
		}
		else if (t.second.type == "unsigned short") {
			cout << t.second.name.nameUShort;
		}
		else if (t.second.type == "unsigned long") {
			cout << t.second.name.nameULong;
		}
		else if (t.second.type == "unsigned long long") {
			cout << t.second.name.nameULongLong;
		}
		cout << "\t" << t.second.type << endl;
	}
}

bool intInfo::insert(const input& t)
{
	idInt in;
	in.type = t.idInt.type;
	if (in.type == "bool") {
		in.name.nameBool = t.idInt.name.nameBool;
	}
	else if (in.type == "int") {
		in.name.nameInt = t.idInt.name.nameInt;
	}
	else if (in.type == "short") {
		in.name.nameShort = t.idInt.name.nameShort;
	}
	else if (in.type == "long") {
		in.name.nameLong = t.idInt.name.nameLong;
	}
	else if (in.type == "long long") {
		in.name.nameLongLong = t.idInt.name.nameLongLong;
	}
	else if (in.type == "unsigned") {
		in.name.nameUInt = t.idInt.name.nameUInt;
	}
	else if (in.type == "unsigned short") {
		in.name.nameUShort = t.idInt.name.nameUShort;
	}
	else if (in.type == "unsigned long") {
		in.name.nameULong = t.idInt.name.nameULong;
	}
	else if (in.type == "unsigned long long") {
		in.name.nameULongLong = t.idInt.name.nameULongLong;
	}
	if (this->symbols.count(hashf(in)) == 0) {
		this->symbols[hashf(in)] = in;
		return true;
	}
	else {
		cerr << "插入失败" << endl;
		return false;
	};
}

string doubleInfo::hashf(idDouble t)
{
	stringstream ssm;
	if (t.type == "double") {
		ssm << t.name.nameD;
	}
	else if (t.type == "float") {
		ssm << t.name.nameF;
	}
	else if (t.type == "long double") {
		ssm << t.name.nameLD;
	}
	string res;
	ssm >> res;
	return res;
}

void doubleInfo::print()const
{
	cout << "浮点常数表\n";
	for (const auto& t : this->symbols) {
		if (t.second.type == "double") {
			cout << t.second.name.nameD;
		}
		else if (t.second.type == "float") {
			cout << t.second.name.nameF;
		}
		else if (t.second.type == "long double") {
			cout << t.second.name.nameLD;
		}
		cout << "\t" << t.second.type << endl;
	}
}

bool doubleInfo::insert(const input& t)
{
	idDouble in;
	in.type = t.idDouble.type;
	if (in.type == "double") {
		in.name.nameD = t.idDouble.name.nameD;
	}
	else if (in.type == "float") {
		in.name.nameF = t.idDouble.name.nameF;
	}
	else if (in.type == "long double") {
		in.name.nameLD = t.idDouble.name.nameLD;
	}

	if (this->symbols.count(hashf(in)) == 0) {
		this->symbols[hashf(in)] = in;
		return true;
	}
	else {
		cerr << "插入失败" << endl;
		return false;
	};
}

string charInfo::hashf(idChar t)
{
	stringstream ssm;
	if (t.type == "char") {
		ssm << t.name.name;
	}
	else if (t.type == "unsigned char") {
		ssm << t.name.nameU;
	}
	else if (t.type == "wchar_t") {
		ssm << t.name.nameW;
	}
	string res;
	ssm >> res;
	return res;
}

void charInfo::print()const
{
	cout << "字符常数表\n";
	for (const auto& t : this->symbols) {
		if (t.second.type == "char") {
			cout << t.second.name.name;
		}
		else if (t.second.type == "unsigned char") {
			cout << t.second.name.nameU;
		}
		else if (t.second.type == "wchar_t") {
			cout << t.second.name.nameW;
		}
		cout << "\t" << t.second.type << endl;
	}
}

bool charInfo::insert(const input& t)
{
	idChar in;
	in.type = t.idChar.type;
	if (in.type == "char") {
		in.name.name = t.idChar.name.name;
	}
	else if (in.type == "unsigned char") {
		in.name.nameU = t.idChar.name.nameU;
	}
	else if (in.type == "wchar_t") {
		in.name.nameW = t.idChar.name.nameW;
	}

	if (this->symbols.count(hashf(in)) == 0) {
		this->symbols[hashf(in)] = in;
		return true;
	}
	else {
		cerr << "插入失败" << endl;
		return false;
	};
}

string&& strInfo::hashf(idStr i)
{
	return move(i.name);
}

void strInfo::print()const
{
	cout << "字符串常数表\n";
	for (const auto& t : this->symbols) {
		cout << t.second.name;
		cout << "\t" << t.second.type << endl;
	}
}

bool strInfo::insert(const input& t)
{
	idStr in;
	in.type = t.idStr.type;
	in.name = t.idStr.name;

	if (this->symbols.count(hashf(in)) == 0) {
		this->symbols[hashf(in)] = in;
		return true;
	}
	else {
		cerr << "插入失败" << endl;
		return false;
	};
}

ostream& operator<<(ostream& out, const Type t)
{
	if (t == Type::const_number_int)
		out << "整型常数";
	else if (t == Type::const_number_double)
		out << "浮点型常数";
	else if (t == Type::const_number_char)
		out << "字符型常数";
	else if (t == Type::const_number_str)
		out << "字符串型常数";
	else if (t == Type::delimiter)
		out << "界符";
	else if (t == Type::identification)
		out << "标识符";
	else if (t == Type::keywords)
		out << "关键字";
	else if (t == Type::operator_symbol)
		out << "运算符";
	else
		out << "NONE";
	return out;
	// TODO: 在此处插入 return 语句
}
