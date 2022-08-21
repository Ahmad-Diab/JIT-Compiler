#include "pljit/Pljit.hpp"
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
using namespace jitcompiler ;
using namespace jitcompiler::management ;
using namespace jitcompiler::syntax ;
using namespace jitcompiler::semantic ;
//---------------------------------------------------------------------------
int main() {
    string code = "PARAM width, height, depth;\n"
                  "VAR volume;\n"
                  "CONST density = 2400;\n"
                  "BEGIN\n"
                  "volume := width * height * depth;\n"
                  "RETURN density * volume\n"
                  "END.";
    CodeManager manager(code) ;
    TokenStream lexicalAnalyzer(&manager) ;
    assert(lexicalAnalyzer.compileCode()) ;
    assert(!manager.isCodeError()) ;
    FunctionDeclaration functionDeclaration(&manager) ;
    assert(functionDeclaration.compileCode(lexicalAnalyzer)) ;
    cout << "---------PARSE TREE---------\n" ;
    cout << functionDeclaration.testDot() << "\n\n" ;
    cout << functionDeclaration.visualizeDot() << "\n\n" ;
    FunctionAST functionAst(&manager) ;
    assert(functionAst.compileCode(functionDeclaration)) ;
    assert(!manager.isCodeError()) ;
    cout << "---------ABSTRACT SYNTAX TREE---------\n";
    cout << functionAst.testDot() << "\n\n" ;
    cout << functionAst.visualizeDot() << "\n\n" ;
    OptimizationVisitor visitor ;
    functionAst.acceptOptimization(visitor) ;
    cout << "---------OPTIMIZED ABSTRACT SYNTAX TREE---------\n";
    cout << functionAst.testDot() << "\n\n" ;
    cout << functionAst.visualizeDot() << "\n\n" ;
    cout << "---------Result ---------\n";
    EvaluationContext evaluationContext({1 , 2 , 3} , functionAst.getSymbolTable()) ;
    auto res = functionAst.evaluate(evaluationContext) ;
    assert(res.has_value()) ;
    cout << res.value() << "\n\n" ;

    return 0 ;
}
//---------------------------------------------------------------------------
