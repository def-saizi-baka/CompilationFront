#include <iostream>
#include <map>
#include "symbolTable.h"
using namespace std;


int main()
{
	input in;
	symbolTable sym;
	in.Type = Type::identification;
	in.id.type = "int";
	in.id.name = "a";
	sym.insert(in);
	in.id.type = "double";
	in.id.name = "b";
	sym.insert(in);
	in.id.type = "unsigned";
	in.id.name = "c";
	sym.insert(in);
	
	in.Type = Type::const_number_int;
	in.idInt.name.nameInt = 1;
	in.idInt.type = "int";
	sym.insert(in);

	in.Type = Type::const_number_double;
	in.idDouble.name.nameD = 1.0;
	in.idDouble.type = "double";
	sym.insert(in);

	in.Type = Type::const_number_char;
	in.idChar.name.name = 'a';
	in.idChar.type = "char";
	sym.insert(in);

	in.Type = Type::const_number_str;
	in.idStr.name = "indsgh";
	in.idStr.type = "string";
	sym.insert(in);
	sym.print();
	return 0;
}