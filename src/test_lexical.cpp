#include "FA/FA.h"
#include "IO/inputBuffer.h"
#include "IO/config.hpp"
#include "Parser.hpp"
#include "parserTree.hpp"

config con;

int main(){
    // string filename = "./FA/regex.txt";
	// FA fa(filename, READ_FILE_BY_LINE);
    // FA dfa = fa.toDFA();
    // dfa.saveDFA("dfamodel");
    int sys_idx, err_idx;

    // for(auto ss : test){
    //     cout << ss << ": " << endl;
    //     dfa.checkStr(ss, sys_idx, err_idx);
    // }
    FA load_dfa;
    load_dfa.loadDFA("dfamodel");
    InputBuffer inputBuffer("test_in.txt");
    vector<int> tokens;
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(!line.length()) continue;
        
        // cout << line << endl;
        vector<int> token = load_dfa.checkStr(line, sys_idx, err_idx);
        tokens.insert(tokens.end(), token.begin(), token.end());
    }
    for(auto token : tokens){
        cout << token << " " ;
    }

    
    parser Pa;
    // Pa.analysis(tokens, )

}