#include "FA.h"
#include "inputBuffer.h"
#include "config.h"
#include "Parser.hpp"
#include "parserTree.hpp"
#include "Item.h"

config con;

void save(FA& dfa,const string modelPath)
{
    string para = "dfamodel";
	FA fa(para, READ_SYMBOLTABLE);
	dfa = fa.toDFA();
    dfa.saveDFA(modelPath);
}

void load(FA& dfa,const string modelPath)
{
    dfa.loadDFA(modelPath);
}

int main(int argc,char** argv)
{
    FA dfa;
    if(argc < 5)
    {
        cout<<"Not correct parameters";
        exit(-1);
    }
    
    string modelChoice = string(argv[1]);
    if(modelChoice == "-s" || modelChoice == "--save"){
        save(dfa,string(argv[2]));
    }else if(modelChoice == "-l" || modelChoice == "--load"){
        load(dfa,string(argv[2]));
    }

    int sys_idx, err_idx;
    // FA dfa;
    // dfa.loadDFA("dfamodel");

    string inputFile = string(argv[3]);
    InputBuffer inputBuffer(inputFile);
    vector<int> tokens;
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(!line.length()) continue;
        
        // cout << line << endl;
        vector<int> token = dfa.checkStr(line, sys_idx, err_idx);
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
    Pa.get_tree().to_json(argv[4]);
}