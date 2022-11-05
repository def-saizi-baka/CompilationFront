#include"inputBuffer.h"
using namespace std;
int main(){
    string path = "testBuffer.cpp";
    InputBuffer inputBuffer(path);
    const char* ss = "34567890p-[\
    4567890-=\
    45678\r\n\"90-=\
    567890";
    // annation
    /**
     * @brief test
     * 
     * 
     * 
     * 
     */
    while(inputBuffer.readline() != InputState::END_OF_FILE){
        string line;
        inputBuffer.pop(line);
        if(line.length())
            cout << line << endl;
    }
}