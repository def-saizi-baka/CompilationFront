#include <iostream>
#include <string>
#include "FA.h"
using namespace std;

int main()
{
	/*string reg = "a*|b*";
	FA fa(reg);*/

	string filename = "regex.txt";
	FA fa(filename, READ_FILE_BY_LINE);

	fa.showFA();

	return 0;
}