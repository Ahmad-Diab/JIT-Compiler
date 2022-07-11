#include <array>
#include <iostream>
#include "lib/CodeManager.h"
#include "lib/LexicalAnalyzer.h"
//---------------------------------------------------------------------------
using namespace std;
using namespace jitcompiler ;
//---------------------------------------------------------------------------

int main() {
    string source_code = "PARAM width , height , depth ; \n VAR volume ; \n CONST density = 2400 ;\nBEGIN\nvolume := width * height * depth ;\nRETURN density * volume\nEND .";
    CodeManager manager (source_code) ;
    LexicalAnalyzer lexicalAnalyzer(&manager) ;
    if(lexicalAnalyzer.isInitialized())
    {
        while (!lexicalAnalyzer.isEmpty())
        {
            LexicalAnalyzer::Token cur = lexicalAnalyzer.getNextToken() ;
            cout << manager.getCurrentLine(cur.line).substr(cur.start_index , cur.last_index - cur.start_index + 1) << ' ';
            cout << cur.type << endl ;
            lexicalAnalyzer.popNextToken() ;
        }
    }
    else
    {
        cout << "hello" ;
    }
}
//---------------------------------------------------------------------------
