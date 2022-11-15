#include "FA.h"
#include "inputBuffer.h"
#include "Parser.hpp"
#include "parserTree.hpp"
#include "Gram.h"
#include "Exception.h"

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
    bool isDebug;
    string unKnownCmd;
    string modelFile;
    string inFile;
    string outFile;
    string lexOutFile;


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
        isDebug = false;
        processType = LEX_GRAMMER;
        modelFile = "./dfamodel";
        inFile = "";
        outFile = "";
        lexOutFile = "";
    }
};

void help()
{
    cout << "usage : "<<endl;
    cout << "-h / --help                                    : get the helpful information"<<endl;
    cout << "-v / --version                                 : get the version information"<<endl;
    cout << "-s / --save    [filepath]                      : save the dfa data for faster startup next time"<<endl;
    cout << "                                                   [filepath] the path where you save dfa data"<<endl;
    cout << "-l / --load    [filepath]                      : load the saved dfa data on disk"<<endl;
    cout << "                                                   use the ${cwd}/dfamodel file defaultly"<<endl;
    cout << "                                                   [filepath] the path where the dfa data saved"<<endl;
    cout << "--lex                                          : only finish the lexical task"<<endl;
    cout << "-i / --infile  [filepath]                      : input the file which need to be parsed"<<endl;
    cout << "                                                   [filepath] the path of input file"<<endl;
    cout << "-go / --gram_outfile [filepath]                : output the grammar parse result as json format"<<endl;
    cout << "                                                   [filepath] the path where the result saved"<<endl;
    cout << "-lo / --lex_outfile [filepath]                 : output the lexical parse result as json format"<<endl;
    cout << "                                                   [filepath] the path where the result saved"<<endl;
    cout << "-d / --debug                                   : get all the debug info" << endl;
    cout << "-pk / --path_keywords [filepath]               : set the path of the keywords configuration file"<<endl;
    cout << "                                                   [filepath] the path where the file saved"<<endl;
    cout << "-pd / --path_delimiters [filepath]             : set the path of the delimiters configuration file"<<endl;
    cout << "                                                   [filepath] the path where the file saved"<<endl;
    cout << "-po / --path_operator_symbols [filepath]       : set the path of the operator symbols configuration file"<<endl;
    cout << "                                                   [filepath] the path where the file saved"<<endl;
    cout << "-pu / --path_unstop_symbols [filepath]         : set the path of the unstop symbols configuration file"<<endl;
    cout << "                                                   [filepath] the path where the file saved"<<endl;
    cout << "-pg / --path_grammar [filepath]                : set the path of the grammar configuration file"<<endl;
    cout << "                                                   [filepath] the path where the file saved"<<endl;
    cout << "-plog / --path_parserlog [filepath]            : set the path of the parser log file"<<endl;
    cout << "                                                   [filepath] the path where the file saved"<<endl;

    cout << "*************************************************************************************"<<endl;
    cout << "you can input cmd like : " << endl;
    cout << "                              main.exe --lex -i in.txt -lo out.txt                       " << endl;
    cout << "                              main.exe -i in.txt -go out.json -lo out.txt                      " << endl;
}

void version()
{
    cout << "Grammer Parse for C-like Programming Language. version: 1.0"<<endl;
    cout << "CopyRight(C) 2022 DH.Chen,ZH.Zhang,Y.Liu, Department of Computer Science, Tongji University." <<endl;
}

void unKnown(string unKnownCmd)
{
    cout<< "command "<<unKnownCmd<<" not found, you can use -h or --help for help."<<endl;
}

void saveDFA(FA& dfa,const string modelPath)
{
    cout << "It will take some time for building dfa from regular expression,please wait ......"<<endl;
    cout << "You can get the regular expression in config/delimiter.txt config/keywords.txt config/operator_symbol.txt "<<endl;
    cout << "If something error in program, we will inform you as quickily as possible"  << endl;
    string para = "dfamodel";
	FA fa(para, READ_SYMBOLTABLE);
	dfa = fa.toDFA();
    dfa.saveDFA(modelPath);
    cout << "Save DFA successfully" <<endl;
}

void loadDFA(FA& dfa,const string modelPath)
{
    cout << "loading the dfa......" <<endl;
    dfa.loadDFA(modelPath);
    cout << "load the dfa successfully"<<endl;
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
        else if(Option == "-go" || Option == "--gram_outfile") {
            ops.outFile = string(argv[++i]);
        }
        else if(Option == "-lo" || Option == "--lex_outfile"){
            ops.lexOutFile = string(argv[++i]);
        }
        else if(Option == "--lex"){
            ops.processType = ONLY_LEX;
        }
        else if(Option == "-d" || Option == "--debug"){
            ops.isDebug = true;
        }
        else if(Option == "-pk" || Option == "--path_keywords"){
            con.path_keyword = string(argv[++i]);
        }
        else if(Option == "-pd" || Option == "--path_delimiters"){
            con.path_delimiter = string(argv[++i]);
        }
        else if(Option == "-po" || Option == "--path_operator_symbols"){
            con.path_operator = string(argv[++i]);
        }
        else if(Option == "-pu" || Option == "--path_unstop_symbols"){
            con.path_unstop = string(argv[++i]);
        }
        else if(Option == "-pg" || Option == "--path_grammar"){
            con.grammar_path = string(argv[++i]);
        }
        else if(Option == "-plog" || Option == "--path_parserlog"){
            con.log_path = string(argv[++i]);
        }
        else{
            ops.unKnown = true;
            ops.unKnownCmd = Option;
        }
    }
}

void lexParse(FA& dfa,string inFile,string outFile)
{
    int sys_idx, err_idx;

    // 词法分析获取到单词表示序列
    string inputFile = inFile;
    InputBuffer inputBuffer(inputFile);
    vector<token> tokens;
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(!line.length()) 
            continue;
        vector<token> _token = dfa.checkStr(line, sys_idx, err_idx,inputBuffer.getLineNumber());
        tokens.insert(tokens.end(), _token.begin(), _token.end());
    }
    tokens.push_back({inputBuffer.getLineNumber(),"$",Config::end_int});
    
    ofstream fout(outFile,ios::out);
    if(!fout.is_open()){
        cerr << "File" + outFile +" not found" <<endl;
        exit(-1); 
    }
    for(auto _token : tokens){
        fout <<_token.value << "\t\t\t" <<_token.symbol<<"\t\t\t" << con.get_name(_token.symbol) << endl;
    }
}

void lexParse(FA& dfa,string inFile,string outFile,vector<token>& tokens,int isDebug)
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
        vector<token> token = dfa.checkStr(line, sys_idx, err_idx, inputBuffer.getLineNumber());
        tokens.insert(tokens.end(), token.begin(), token.end());
    }
    tokens.push_back({inputBuffer.getLineNumber(),"$",Config::end_int});

    if(outFile != ""){
        ofstream fout(outFile,ios::out);
        if(!fout.is_open()){
            cerr << "File" + outFile +" not found" <<endl;
            exit(-1); 
        }
        for(auto _token : tokens){
            fout <<_token.value << "\t\t\t" <<_token.symbol<<"\t\t\t" << con.get_name(_token.symbol) << endl;
        }
    }
    
    if(isDebug){
        cout << "******************************************************************************************************" <<endl;
        cout << "                         This is the lexcial result :                                                 " <<endl;
        cout << "******************************************************************************************************" <<endl;
        for(auto _token : tokens){
            cout <<_token.value << "\t\t\t" <<_token.symbol<<"\t\t\t" << con.get_name(_token.symbol) << endl;
        }
    }
}

void gramParse(FA& dfa,string inFile,string outFile,string lexOutFile, int isDebug)
{
    int sys_idx, err_idx;

    vector<token> tokens;
    lexParse(dfa,inFile,lexOutFile,tokens,isDebug);

    // 语法分析，以json的格式输出语法树
    CFG cfg;
    // 设置是否打印过程信息
    if(isDebug){
        cfg.setDebug();
    }
    cout << endl;
    cout << "building the ACTION table and GOTO table"<<endl;
    cfg.initItems();
    cfg.initLRItems();
	cfg.formFirstSet();
    cfg.buildClosures();
    cfg.buildAnalysisTable();
    map<int, std::vector<std::pair<int, int>>> analysisTable = cfg.getAnalysisTable();
    cout << "build the ACTION table and GOTO table successfully!" << endl;

    parser Pa;
    if(isDebug)
    {
        cout << "******************************************************************************************************" <<endl;
        cout << "                         移进归约过程如下 :                                                 " <<endl;
        cout << "******************************************************************************************************" <<endl;
    }
    Pa.setDebug(isDebug);
    cout << endl;
    cout << "Analysing the C-like code..."<<endl;
    Pa.analysis(tokens, analysisTable);
    Pa.get_tree().to_json(outFile);
    cout << "Analyse the C-like code successfully!"<<endl;
    cout << "The grammar tree has been saved in " + outFile << endl;
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
            ops.inFile= "./test_in.txt";
        }else if(ops.outFile == ""){
            ops.outFile = "tree.json";
        }

        if(ops.processType == LEX_GRAMMER){
            // 执行语法分析
            gramParse(dfa,ops.inFile,ops.outFile,ops.lexOutFile,ops.isDebug);
        }else{
            lexParse(dfa,ops.inFile,ops.lexOutFile);
        }

    }
}

int main(int argc,char** argv)
{
    try{
        cmdOptions ops;
        cmdParse(argc,argv,ops);
        con.init();
        optionEXE(ops);
        return 0;
    }
    catch(parserException& t){
        cerr << t.what()  << endl;
        return -2;
    }
    catch(lexException& t){
        cerr << t.what()  << endl;
        return -2;    
    }
}
