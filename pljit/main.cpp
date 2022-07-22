#include "lib/CodeManager.h"
#include "lib/TokenStream.h"
#include "pljit/lib/ParseTree.h"
#include <array>
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
using namespace jitcompiler ;
//---------------------------------------------------------------------------

int main() {
    string source_code = "PARAM width , height , depth ; \n VAR volume ; \n CONST density = 2400 ;\nBEGIN\nvolume := width * height * depth ;\nRETURN density * volume\nEND .";
    CodeManager manager (source_code) ;
    TokenStream lexicalAnalyzer(&manager) ;
    if(lexicalAnalyzer.isInitialized())
    {
        while (!lexicalAnalyzer.isEmpty())
        {
            TokenStream::Token cur = lexicalAnalyzer.getNextToken() ;
            cout << manager.getCurrentLine(cur.line).substr(cur.start_index , cur.last_index - cur.start_index + 1) << ' ';
            cout << cur.type << endl ;
            lexicalAnalyzer.popNextToken() ;
        }
    }
    else
    {
        cout << "hello" ;
    }
    lexicalAnalyzer.reset() ;
    unique_ptr<FunctionDeclaration> parseTreeNode = make_unique<FunctionDeclaration>(&manager , &lexicalAnalyzer) ;
    if(parseTreeNode->recursiveDecentParser())
    {
        cout << "Syntax Analysis succeed" << "\n";
    }
    else
    {
        cout << "Syntax Analysis failed" << "\n";
    }
}
//---------------------------------------------------------------------------
