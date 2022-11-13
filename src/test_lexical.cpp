#include "FA.h"
#include "inputBuffer.h"
#include "config.h"
#include "Parser.hpp"
#include "parserTree.hpp"
#include "Item.h"

config con;

int main(){
    // string path = "dfghjk";
	// FA fa(path, READ_SYMBOLTABLE);
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
    tokens.push_back(Config::end_int);
    for(auto token : tokens){
        cout << token << " " ;
    }

    CFG cfg;
        // 初始化项目
    // cfg.setDebug();
    cfg.initItems();
    cfg.initLRItems();
	cfg.formFirstSet();
    cfg.buildClosures();
    cfg.buildAnalysisTable();
    map<int, std::vector<std::pair<int, int>>> analysisTable = cfg.getAnalysisTable();
    
    parser Pa;
    Pa.analysis(tokens, analysisTable);
    

}