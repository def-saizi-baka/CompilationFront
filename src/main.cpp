#include "FA.h"
#include "inputBuffer.h"
#include "config.h"
#include "Parser.hpp"
#include "parserTree.hpp"
#include "Item.h"

#define ONLY_LEX    0
#define LEX_GRAMMER 1

config con;

struct cmdOptions{
    bool isHelp;
    bool isVersion;
    bool isSave;
    bool isLoad;
    bool unKnown;
    int  processType;
    string unKnownCmd;
    string modelFile;
    string inFile;
    string outFile;


    cmdOptions()
    {
        // 默认使用自带的dfa模型加载
        // 默认进行词法+语法分析
        unKnown = false;
        unKnownCmd = "";
        isHelp = false;
        isVersion = false;
        isSave = false;
        isLoad = true;
        processType = LEX_GRAMMER;
        modelFile = "dfamodel";
        inFile = "";
        outFile = "";
    }
};

void help()
{
    cout << "usage : "<<endl;
    cout << "-h / --help                    : get the helpful information"<<endl;
    cout << "-v / --version                 : get the version information"<<endl;
    cout << "-s / --save    [filepath]      : save the dfa data for faster startup next time"<<endl;
    cout << "                                   [filepath] the path where you save dfa data"<<endl;
    cout << "-l / --load    [filepath]      : load the saved dfa data on disk"<<endl;
    cout << "                                   use the ${cwd}/dfamodel file defaultly"<<endl;
    cout << "                                   [filepath] the path where the dfa data saved"<<endl;
    cout << "-i / --infile  [filepath]      : input the file which need to be parsed"<<endl;
    cout << "                                   [filepath] the path of input file"<<endl;
    cout << "-o / --outfile [filepath]      : output the parse result as json format"<<endl;
    cout << "                                   [filepath] the path where the result saved"<<endl;
    cout << "*************************************************************************************"<<endl;
    cout << "you can input cmd like : " << endl;
    cout << "                              main.exe -i in.txt -o out.json                       " << endl;
}

void version()
{
    cout << "Grammer Parse for C-like Programming Language. version: 1.0"<<endl;
    cout << "CopyRight(C) 2022 DH.Chen,ZH.Zhang,Y.Liu, Department of Computer Science, Tongji University." <<endl;
}

void unKnown(string unKnownCmd)
{
    cout<< "command "<<unKnownCmd<<" not found, you can use -h or --help for hlep."<<endl;
}

void saveDFA(FA& dfa,const string modelPath)
{
    cout << "It will take some time for building dfa from regular expression,please wait ......"<<endl;
    cout << "you can get the regular expression in config/delimiter.txt config/keywords.txt config/operator_symbol.txt "<<endl;
    cout << "If something error in program, we will inform you as quickily as possible"  << endl;
    string para = "dfamodel";
	FA fa(para, READ_SYMBOLTABLE);
	dfa = fa.toDFA();
    dfa.saveDFA(modelPath);
}

void loadDFA(FA& dfa,const string modelPath)
{
    cout << "loading the dfa......" <<endl;
    dfa.loadDFA(modelPath);
}

void cmdParse(int argc,char** argv,cmdOptions& ops)
{
    for(int i=1;i<argc;i++)
    {
        string Option(argv[i]);
        cout << Option << endl;
        if(Option == "-h" || Option == "--help") {
            ops.isHelp = true;
        }
        else if(Option == "-v" || Option == "--version") {
            ops.isVersion = true;
        }
        else if(Option == "-s" || Option == "--save") {
            ops.isSave = true;
            ops.modelFile = string(argv[++i]);
        }
        else if(Option == "-l" || Option == "--load"){
            ops.isLoad = true;
            ops.modelFile = string(argv[++i]);
        }
        else if(Option == "-i" || Option == "--infile"){
            ops.inFile = string(argv[++i]);
        } 
        else if(Option == "-o" || Option == "--outfile") {
            ops.outFile = string(argv[++i]);
        }else if(Option == "--lex"){
            ops.processType = ONLY_LEX;
        }
        else{
            ops.unKnown = true;
            ops.unKnownCmd = Option;
        }
    }
}

vector<int> lexParse(FA& dfa,string inFile,string outFile)
{
    int sys_idx, err_idx;

    // 词法分析获取到单词表示序列
    string inputFile = inFile;
    InputBuffer inputBuffer(inputFile);
    vector<int> tokens;
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(!line.length()) 
            continue;
        vector<int> token = dfa.checkStr(line, sys_idx, err_idx);
        tokens.insert(tokens.end(), token.begin(), token.end());
    }
    tokens.push_back(Config::end_int);
    for(auto token : tokens){
        cout << token << " " ;
    }
    return tokens;
}

void lexParse(FA& dfa,string inFile,string outFile,vector<int>& tokens)
{
    int sys_idx, err_idx;
    // 词法分析获取到单词表示序列
    string inputFile = inFile;
    InputBuffer inputBuffer(inputFile);
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(!line.length()) 
            continue;
        vector<int> token = dfa.checkStr(line, sys_idx, err_idx);
        tokens.insert(tokens.end(), token.begin(), token.end());
    }
    tokens.push_back(Config::end_int);
    for(auto token : tokens){
        cout << token << " " ;
    }
}

void gramParse(FA& dfa,string inFile,string outFile)
{
    int sys_idx, err_idx;

    vector<int> tokens;
    lexParse(dfa,inFile,outFile,tokens);

    // 语法分析，以json的格式输出语法树
    CFG cfg;
    // 设置是否打印过程信息
    // cfg.setDebug();
    cfg.initItems();
    cfg.initLRItems();
	cfg.formFirstSet();
    cfg.buildClosures();
    cfg.buildAnalysisTable();
    map<int, std::vector<std::pair<int, int>>> analysisTable = cfg.getAnalysisTable();
    
    parser Pa;
    Pa.analysis(tokens, analysisTable);
    Pa.get_tree().to_json(outFile);
}

void optionEXE(cmdOptions& ops)
{
    FA dfa;

    if(ops.unKnown)
    {
        unKnown(ops.unKnownCmd);
    }else{
        if(ops.isHelp){
            help();
            return ;
        }
        if(ops.isVersion){
            version();
            return ;
        }
        
        // dfa数据的加载和保存
        if(ops.isSave){
            saveDFA(dfa,ops.modelFile);
        }
        if(ops.isLoad){
            loadDFA(dfa,ops.modelFile);
        }

        //
        if(ops.inFile == ""){
            ops.inFile= "test_in.txt";
        }else if(ops.outFile == ""){
            ops.outFile = "tree.json";
        }

        if(ops.processType == LEX_GRAMMER){
            // 执行语法分析
            gramParse(dfa,ops.inFile,ops.outFile);
        }else{
            lexParse(dfa,ops.inFile,ops.outFile);
        }

    }
}

int main(int argc,char** argv)
{
    cmdOptions ops;
    cmdParse(argc,argv,ops);
    optionEXE(ops);
    return 0;
}
