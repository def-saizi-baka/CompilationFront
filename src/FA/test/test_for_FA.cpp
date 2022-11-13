#include <iostream>
#include <string>
#include "FA.h"
#include "config.h"
using namespace std;

config con;

void save(FA& dfa)
{
    string para = "nothing here";
	FA fa(para, READ_SYMBOLTABLE);
	dfa = fa.toDFA();
    dfa.saveDFA("dfamodel");
}

void load(FA& dfa)
{
    dfa.loadDFA("dfamodel");
}

int main(int argc,char** argv)
{
    
    FA dfa;
    // string para = "nothing here";
	// FA fa(para, READ_SYMBOLTABLE);
	// dfa = fa.toDFA();
    // dfa.saveDFA("dfamodel");
    if(argc <2)
    {
        cout<<"You need give 0 for save,1 for load";
        exit(-1);
    }
    if(string(argv[1]) == "1"){
        load(dfa);
    }else if(string(argv[1]) == "0"){
        save(dfa);
    }

    int sys_idx, err_idx;
    int ParaNum = 2;
    while(ParaNum < argc)
    {
        string ss(argv[ParaNum]);
        ++ParaNum;
        cout << ss << ": " << endl;
        dfa.checkStr(ss, sys_idx, err_idx);
    }
    // string test[] = {
    //     "inti", 
    //     "int",
    //     "int  a++  ",
    //     "main()",
    //     "main",
    //     //"\"12324342\"",
    //     "123"
    // };
    // for(auto ss : test){
    //     cout << ss << ": " << endl;
    //     dfa.checkStr(ss, sys_idx, err_idx);
    // }
	return 0;
}