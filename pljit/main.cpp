#include "EvaluationContext.hpp"
#include "OptimizationASTVisitor.hpp"
#include "Pljit.hpp"
#include "PrintASTVistor.hpp"
#include "PrintParseTreeVisitor.hpp"
#include <iostream>

//---------------------------------------------------------------------------
using namespace std;
using namespace jitcompiler ;
//---------------------------------------------------------------------------

void foo1()
{
    string identifier = "volume";
    CodeManager manager(identifier) ;
    TokenStream lexicalAnalyzer(&manager);
    lexicalAnalyzer.compileCode() ;
    PrimaryExpression primaryExpression(&manager) ;
    if(!primaryExpression.recursiveDecentParser(lexicalAnalyzer))
    {
        cout << "error\n" ;
    }
    else {
        PrintVisitor<false> printVisitor ;
        primaryExpression.accept(printVisitor) ;
        cout << printVisitor.getOutput()  ;
    }
}

int main() {
    foo1() ;
//    string source_code1 = "PARAM  d;\n"
//                         "VAR volume;\n"
//                         "CONST density=2400,width=21,height=3,depth=2;\n"
//                         "BEGIN\n"
//                         "volume := width * height * depth;\n"
//                         "RETURN density * volume + d\n"
//                         "END." ;
//    string source_code2 = "PARAM width, height, depth;\n"
//                          "VAR volume;\n"
//                          "CONST density = 2400;\n"
//                          "BEGIN\n"
//                          "volume := width * height * depth;\n"
//                          "RETURN density * volume\n"
//                          "END.";

//    Pljit pljit ;
//    auto f1 = pljit.registerFunction(source_code1) ;
//    auto f2 = pljit.registerFunction(source_code2) ;
//    vector<int64_t> parameter_list1 = {1};
//    vector<int64_t> parameter_list2 = {1 , 2 , 3};
//    vector<int64_t> parameter_list3 = {10 , 1 , 2};
//    cout << f1(parameter_list1).value() << '\n' ;
//    cout << f2(parameter_list2).value() << '\n' ;
//    cout << f2(parameter_list3).value() << '\n' ;

//    CodeManager manager (source_code2) ;
//    TokenStream lexicalAnalyzer(&manager) ;
//    lexicalAnalyzer.compileCode();
//    if(!manager.isCodeError()) {
//        while (!lexicalAnalyzer.isEmpty()) {
//            TokenStream::Token token = lexicalAnalyzer.lookup() ;
//            string_view line = manager.getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
//            size_t start_index = token.getCodeReference().getStartLineRange().second ;
//            size_t last_index = token.getCodeReference().getEndLineRange().second ;
//
//            cout << line.substr(start_index , last_index - start_index + 1) << ' ';
//            cout << token.getTokenType() << endl ;
//            lexicalAnalyzer.nextToken() ;
//        }
//    }
//    else
//    {
//        cout << "lexical error" << endl ;
//        cout << manager.error_message() << endl ;
//        return 1 ;
//    }
//    lexicalAnalyzer.reset() ;
//    unique_ptr<FunctionDeclaration> parseTreeNode = make_unique<FunctionDeclaration>(&manager) ;
//    if(parseTreeNode->recursiveDecentParser(lexicalAnalyzer))
//    {
//        cout << "Syntax Analysis succeed" << "\n";
//
//        PrintVisitor printVisitor ;
//        parseTreeNode->accept(printVisitor);
//        cout << "digraph {\n" ;
//        cout << printVisitor.getOutput()  ;
//        cout << "}\n" ;
//        cout << "--------------------------------\n" ;
//        cout << '\n' ;
//        FunctionAST functionAst(parseTreeNode , &manager) ;
////
//
////        vector<int64_t> parameter_list = {3 , 1 , 2};
//        EvaluationContext evaluationContext (functionAst.getSymbolTable()) ;
//        OptimizationVisitor optimizationVisitor(evaluationContext) ;
//        optimizationVisitor.visitOptimization(functionAst) ;
//        PrintASTVistor  printAstVistor ;
//        functionAst.accept(printAstVistor) ;
//        cout << "digraph {\n" ;
//        cout << printAstVistor.getOutput() ;
//        cout << "}\n" ;
//        cout << "--------------------------------\n" ;
////        EvaluationContext evaluationContext1({1} , functionAst.getSymbolTable()) ;
////        auto result = functionAst.evaluate(evaluationContext1);
////        if(!result.has_value())
////            cout << "{}" << '\n' ;
////        else
////            cout << result.value() << '\n' ;
////
//    }
//    else
//    {
//        cout << "Syntax Analysis failed" << "\n";
//        cout << manager.error_message() << '\n' ;
//    }
}
//---------------------------------------------------------------------------
