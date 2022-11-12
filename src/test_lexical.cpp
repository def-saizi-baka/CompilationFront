#include "FA/FA.h"
#include "IO/inputBuffer.h"

int main(){
    string filename = "regex.txt";
	FA fa(filename, READ_FILE_BY_LINE);
    FA dfa = fa.toDFA();
    int sys_idx, err_idx;

    InputBuffer inputBuffer("test_in.txt");
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(!line.length()) continue;
        
        cout << line << endl;
        dfa.checkStr(line, sys_idx, err_idx);

    }
}