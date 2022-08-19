#include <gtest/gtest.h>

#include "pljit/AST.hpp"
#include "pljit/CodeManager.hpp"
#include "pljit/ParseTree.hpp"
#include "pljit/TokenStream.hpp"
#include "pljit/OptimizationASTVisitor.hpp"
#include "pljit/PrintASTVisitor.hpp"

using namespace std ;
using namespace jitcompiler ;

TEST(TestOptimization , TestDeadCodeElimination)
{
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