#include <gtest/gtest.h>

#include "pljit/management/CodeManager.hpp"
#include "pljit/semantic/AST.hpp"
#include "pljit/semantic/OptimizationASTVisitor.hpp"
#include "pljit/semantic/PrintASTVisitor.hpp"
#include "pljit/syntax/ParseTree.hpp"
#include "pljit/syntax/TokenStream.hpp"

using namespace std ;
using namespace jitcompiler ;
using namespace jitcompiler ::management;
using namespace jitcompiler ::syntax;
using namespace jitcompiler ::semantic;

TEST(TestOptimization , TestDeadCodeElimination) {
    constexpr string_view code =
        "PARAM xa , xb , ya , yb;\n"
        "VAR x , y , d;\n"
        "BEGIN\n"
        "x := (xa - xb) * (xa - xb);\n"
        "y := (ya - yb) * (ya - yb);\n"
        "d := x + y;\n"
        "RETURN d;\n"
        "RETURN x + y\n"
        "END.\n" ;
    constexpr string_view oldDot =  "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"xa\";\n"
                                   "\tIdentifier -> \"xb\";\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"xa\";\n"
                                   "\tIdentifier -> \"xb\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"ya\";\n"
                                   "\tIdentifier -> \"yb\";\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"ya\";\n"
                                   "\tIdentifier -> \"yb\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tIdentifier -> \"x\";\n"
                                   "\tIdentifier -> \"y\";\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"d\";\n"
                                   "\tReturn Statement -> BinaryExpression(+);\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tIdentifier -> \"x\";\n"
                                   "\tIdentifier -> \"y\";\n"
                                   "}\n" ;
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                         "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                         "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tIdentifier -> \"xa\";\n"
                                         "\tIdentifier -> \"xb\";\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tIdentifier -> \"xa\";\n"
                                         "\tIdentifier -> \"xb\";\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                         "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                         "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tIdentifier -> \"ya\";\n"
                                         "\tIdentifier -> \"yb\";\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tIdentifier -> \"ya\";\n"
                                         "\tIdentifier -> \"yb\";\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
                                         "\tBinaryExpression(+) -> Identifier;\n"
                                         "\tBinaryExpression(+) -> Identifier;\n"
                                         "\tIdentifier -> \"x\";\n"
                                         "\tIdentifier -> \"y\";\n"
                                         "\tReturn Statement -> Identifier;\n"
                                         "\tIdentifier -> \"d\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());
    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;
    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;
    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
TEST(TestOptimization , TestConstantPropagationIdentifier)
{
    constexpr string_view code = "CONST a = 1;\n"
                                 "BEGIN\n"
                                 "RETURN a\n"
                                 "END.\n"

        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"1\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , 1) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;

}

TEST(TestOptimization , TestConstantPropagationMinus)
{
    constexpr string_view code = "VAR b;"
                                 "CONST a = 1;\n"
                                 "BEGIN\n"
                                 "RETURN -a\n"
                                 "END.\n"

        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tReturn Statement-> UnaryExpression(-);\n"
                                   "\tUnaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"-1\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , -1) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;

}
TEST(TestOptimization , TestConstantPropagationUnaryPlus)
{
    constexpr string_view code = "VAR b;"
                                 "CONST a = 1;\n"
                                 "BEGIN\n"
                                 "RETURN +a\n"
                                 "END.\n"

        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tReturn Statement-> UnaryExpression(+);\n"
                                   "\tUnaryExpression(+) -> Identifier;\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"1\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , 1) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
TEST(TestOptimization , TestConstantPropagationBinaryPlus)
{
    constexpr string_view code = "VAR b;"
                                 "CONST a = 1;\n"
                                 "BEGIN\n"
                                 "b := a + 12;\n"
                                 "RETURN b\n"
                                 "END.\n"

        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tBinaryExpression(+) -> Literal;\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "\tLiteral -> \"12\";\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"b\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"13\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , 13) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
TEST(TestOptimization , TestConstantPropagationBinaryMinus)
{
    constexpr string_view code = "VAR b;"
                                 "CONST a = 2;\n"
                                 "BEGIN\n"
                                 "b := a - 12;\n"
                                 "RETURN b\n"
                                 "END.\n"
        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Literal;\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "\tLiteral -> \"12\";\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"b\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"-10\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , -10) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
TEST(TestOptimization , TestConstantPropagationBinaryMult)
{
    constexpr string_view code = "VAR b;"
                                 "CONST a = 3;\n"
                                 "BEGIN\n"
                                 "b := a * -12;\n"
                                 "RETURN b\n"
                                 "END.\n"
        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                   "\tBinaryExpression(*) -> Identifier;\n"
                                   "\tBinaryExpression(*)-> UnaryExpression(-);\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "\tUnaryExpression(-) -> Literal;\n"
                                   "\tLiteral -> \"12\";\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"b\";\n"
                                   "}\n" ;
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"-36\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , -36) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
TEST(TestOptimization , TestConstantPropagationBinaryDiv)
{
    constexpr string_view code = "VAR b;"
                                 "CONST a = 36;\n"
                                 "BEGIN\n"
                                 "b := a / -12;\n"
                                 "RETURN b\n"
                                 "END.\n"
        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(/);\n"
                                   "\tBinaryExpression(/) -> Identifier;\n"
                                   "\tBinaryExpression(/)-> UnaryExpression(-);\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "\tUnaryExpression(-) -> Literal;\n"
                                   "\tLiteral -> \"12\";\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"b\";\n"
                                   "}\n" ;
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Literal;\n"
                                         "\tLiteral -> \"-3\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , -3) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
TEST(TestOptimization , TestConstantPropagationBinaryDivByZero)
{
    constexpr string_view code = "VAR b , c , f;\n"
                                 "CONST a = 3 , d = 12;\n"
                                 "BEGIN\n"
                                 "c := a * d;\n"
                                 "b := c - 36;\n"
                                 "f := 12 / b;\n"
                                 "RETURN f / b;\n"
                                 "RETURN 0\n"
                                 "END.\n"
        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                   "\tBinaryExpression(*) -> Identifier;\n"
                                   "\tBinaryExpression(*) -> Identifier;\n"
                                   "\tIdentifier -> \"a\";\n"
                                   "\tIdentifier -> \"d\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Literal;\n"
                                   "\tIdentifier -> \"c\";\n"
                                   "\tLiteral -> \"36\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(/);\n"
                                   "\tBinaryExpression(/) -> Literal;\n"
                                   "\tBinaryExpression(/) -> Identifier;\n"
                                   "\tLiteral -> \"12\";\n"
                                   "\tIdentifier -> \"b\";\n"
                                   "\tReturn Statement -> BinaryExpression(/);\n"
                                   "\tBinaryExpression(/) -> Identifier;\n"
                                   "\tBinaryExpression(/) -> Identifier;\n"
                                   "\tIdentifier -> \"f\";\n"
                                   "\tIdentifier -> \"b\";\n"
                                   "\tReturn Statement -> Literal;\n"
                                   "\tLiteral -> \"0\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> Literal;\n"
                                         "\tLiteral -> \"36\";\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> Literal;\n"
                                         "\tLiteral -> \"0\";\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> BinaryExpression(/);\n"
                                         "\tBinaryExpression(/) -> Literal;\n"
                                         "\tBinaryExpression(/) -> Literal;\n"
                                         "\tLiteral -> \"12\";\n"
                                         "\tLiteral -> \"0\";\n"
                                         "\tReturn Statement -> BinaryExpression(/);\n"
                                         "\tBinaryExpression(/) -> Identifier;\n"
                                         "\tBinaryExpression(/) -> Literal;\n"
                                         "\tIdentifier -> \"f\";\n"
                                         "\tLiteral -> \"0\";\n"
                                         "}\n";
    constexpr string_view expectedRuntimeError = "6:9: Runtime Error: Divide by Zero\n"
                                                 "f := 12 / b;\n"
                                                 "        ^\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    EvaluationContext evaluationContextOld {functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(!oldEval.has_value()) ;
    ASSERT_EQ(expectedRuntimeError , manager.runtimeErrorMessage()) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(!optimizedEval.has_value()) ;
    ASSERT_EQ(expectedRuntimeError , manager.runtimeErrorMessage()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}

TEST(TestOptimization , TestNestedOptimizations)
{
    constexpr string_view code = "PARAM yb;\n"
                                 "VAR x , y , d;\n"
                                 "CONST xa = 1 , xb = 2 , ya = 3;\n"
                                 "BEGIN\n"
                                 "x := (xa - xb) * (xa - xb);\n"
                                 "y := (ya - yb) * (ya - yb);\n"
                                 "d := x + y;\n"
                                 "RETURN d;\n"
                                 "x:= 10 + (3 - 11) * 11 / 0\n"
                                 "END.\n"

        ;
    constexpr string_view oldDot = "digraph {\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tFunction -> Return Statement;\n"
                                   "\tFunction -> Assignment Statement(:=);\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"xa\";\n"
                                   "\tIdentifier -> \"xb\";\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"xa\";\n"
                                   "\tIdentifier -> \"xb\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"ya\";\n"
                                   "\tIdentifier -> \"yb\";\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tBinaryExpression(-) -> Identifier;\n"
                                   "\tIdentifier -> \"ya\";\n"
                                   "\tIdentifier -> \"yb\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tBinaryExpression(+) -> Identifier;\n"
                                   "\tIdentifier -> \"x\";\n"
                                   "\tIdentifier -> \"y\";\n"
                                   "\tReturn Statement -> Identifier;\n"
                                   "\tIdentifier -> \"d\";\n"
                                   "\tAssignment Statement(:=) -> Identifier;\n"
                                   "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
                                   "\tBinaryExpression(+) -> Literal;\n"
                                   "\tBinaryExpression(+) -> BinaryExpression(*);\n"
                                   "\tLiteral -> \"10\";\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                   "\tBinaryExpression(*) -> BinaryExpression(/);\n"
                                   "\tBinaryExpression(-) -> Literal;\n"
                                   "\tBinaryExpression(-) -> Literal;\n"
                                   "\tLiteral -> \"3\";\n"
                                   "\tLiteral -> \"11\";\n"
                                   "\tBinaryExpression(/) -> Literal;\n"
                                   "\tBinaryExpression(/) -> Literal;\n"
                                   "\tLiteral -> \"11\";\n"
                                   "\tLiteral -> \"0\";\n"
                                   "}\n";
    constexpr string_view optimizedDot = "digraph {\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Assignment Statement(:=);\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> Literal;\n"
                                         "\tLiteral -> \"1\";\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> BinaryExpression(*);\n"
                                         "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                         "\tBinaryExpression(*) -> BinaryExpression(-);\n"
                                         "\tBinaryExpression(-) -> Literal;\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tLiteral -> \"3\";\n"
                                         "\tIdentifier -> \"yb\";\n"
                                         "\tBinaryExpression(-) -> Literal;\n"
                                         "\tBinaryExpression(-) -> Identifier;\n"
                                         "\tLiteral -> \"3\";\n"
                                         "\tIdentifier -> \"yb\";\n"
                                         "\tAssignment Statement(:=) -> Identifier;\n"
                                         "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
                                         "\tBinaryExpression(+) -> Literal;\n"
                                         "\tBinaryExpression(+) -> Identifier;\n"
                                         "\tLiteral -> \"1\";\n"
                                         "\tIdentifier -> \"y\";\n"
                                         "\tReturn Statement -> Identifier;\n"
                                         "\tIdentifier -> \"d\";\n"
                                         "}\n" ;
    CodeManager manager(code);
    TokenStream tokenStream(&manager);
    tokenStream.compileCode();
    ASSERT_TRUE(!manager.isCodeError());
    FunctionDeclaration functionDeclaration(&manager);
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream));
    ASSERT_TRUE(!manager.isCodeError());
    FunctionAST functionAst(&manager);
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration));
    ASSERT_TRUE(!manager.isCodeError());

    vector<int64_t> param = {10};
    EvaluationContext evaluationContextOld {param , functionAst.getSymbolTable()};
    auto oldEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(oldEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , 50) ;

    TestPrintASTVisitor printAstVisitorNonOptimized ;
    functionAst.accept(printAstVisitorNonOptimized) ;
    ASSERT_EQ(printAstVisitorNonOptimized.getOutput() , oldDot) ;
    ASSERT_NE(printAstVisitorNonOptimized.getOutput() , optimizedDot) ;

    OptimizationVisitor optimizationVisitor ;
    functionAst.acceptOptimization(optimizationVisitor) ;
    TestPrintASTVisitor printAstVisitorOptimized ;
    functionAst.accept(printAstVisitorOptimized) ;

    EvaluationContext evaluationContextOptimized {functionAst.getSymbolTable()};
    auto optimizedEval = functionAst.evaluate(evaluationContextOld) ;
    ASSERT_TRUE(optimizedEval.has_value()) ;
    ASSERT_EQ(oldEval.value() , optimizedEval.value()) ;

    ASSERT_NE(printAstVisitorOptimized.getOutput() , oldDot) ;
    ASSERT_EQ(printAstVisitorOptimized.getOutput() , optimizedDot) ;
}
