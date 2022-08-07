#include "AST.h"
#include "CodeManager.h"
#include "EvaluationContext.h"
#include "PrintASTVistor.h"
#include "PrintParseTreeVisitor.h"
#include "TokenStream.h"
#include "pljit/ParseTree.h"
#include <array>
#include <iostream>

//---------------------------------------------------------------------------
using namespace std;
using namespace jitcompiler ;
//---------------------------------------------------------------------------

int main() {
    string source_code = "PARAM width, height, depth;\n"
                         "VAR volume;\n"
                         "CONST density=2400;\n"
                         "BEGIN\n"
                         "volume := width * height * depth;\n"
                         "RETURN density * volume\n"
                         "END." ;
    CodeManager manager (source_code) ;
    TokenStream lexicalAnalyzer(&manager) ;
    if(!manager.isCodeError())
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
        cout << "lexical error" << endl ;
        cout << manager.error_message() << endl ;
        return 1 ;
    }
    lexicalAnalyzer.reset() ;
    unique_ptr<FunctionDeclaration> parseTreeNode = make_unique<FunctionDeclaration>(&manager , &lexicalAnalyzer) ;
    if(parseTreeNode->recursiveDecentParser())
    {
        cout << "Syntax Analysis succeed" << "\n";

        PrintVisitor printVisitor ;
        parseTreeNode->accept(printVisitor);
        cout << "digraph {\n" ;
        cout << printVisitor.getOutput()  ;
        cout << "}\n" ;
        cout << "--------------------------------\n" ;
        cout << '\n' ;
        FunctionAST functionAst(parseTreeNode , &manager) ;
        PrintASTVistor  printAstVistor ;
//        functionAst.accept(printAstVistor) ;
//        cout << "digraph {\n" ;
//        cout << printAstVistor.getOutput() ;
//        cout << "}\n" ;
//
        vector<int64_t> parameter_list = {3 , 1 , 2};
        EvaluationContext evaluationContext (parameter_list , functionAst.getSymbolTable()) ;
        cout << functionAst.evaluate(evaluationContext).value() << '\n' ;
    }
    else
    {
        cout << "Syntax Analysis failed" << "\n";
        cout << manager.error_message() << '\n' ;
    }
}
//---------------------------------------------------------------------------
