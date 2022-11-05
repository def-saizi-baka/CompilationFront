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
    // fa.showFA();

	FA dfa = fa.toDFA();
    dfa.showFA();
    int sys_idx, err_idx;
    string test[] = {
        "inti", 
        "int",
        "int  a++  ",
        "main()",
        "main"
    };
    for(auto ss : test){
        cout << ss << ": " << endl;
        dfa.checkStr(ss, sys_idx, err_idx);
    }
	return 0;
}