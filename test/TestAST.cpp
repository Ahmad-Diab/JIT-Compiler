#include <gtest/gtest.h>

#include "pljit/CodeManager.hpp"
#include "pljit/TokenStream.hpp"
#include "pljit/ParseTree.hpp"
#include "pljit/AST.hpp"
#include "pljit/PrintASTVisitor.hpp"

using namespace std ;
using namespace jitcompiler ;

TEST(TestAST , TestSymbolTable)
{
    {
        constexpr string_view code = "PARAM a , b , c;\n"
                                     "VAR d , e , f;\n"
                                     "CONST g=1,h=2,i=3;\n"
                                     "BEGIN\n"
                                     "RETURN 0\n"
                                     "END\n"
                                     ".\n" ;

        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        SymbolTable symbolTable(&manager , functionDeclaration) ;
        ASSERT_TRUE(symbolTable.isComplied()) ;
    }
    { // error same identifier being declared twice
        constexpr string_view code = "PARAM a , b , c;\n"
                                     "VAR a , e , f;\n"
                                     "CONST g=1,h=2,i=3;\n"
                                     "BEGIN\n"
                                     "RETURN 0\n"
                                     "END\n"
                                     ".\n"  ;
        constexpr string_view expected = "2:5: error: Already declared\n"
                                         "VAR a , e , f;\n"
                                         "    ^\n" ;
        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        SymbolTable symbolTable(&manager , functionDeclaration) ;
        ASSERT_TRUE(!symbolTable.isComplied()) ;
        ASSERT_TRUE(manager.isCodeError()) ;
        ASSERT_EQ(expected , manager.error_message()) ;
    }
}

TEST(TestAST , TestLiteral) {
    constexpr string_view code = "BEGIN\n"
                                 "RETURN 1\n"
                                 "END\n"
                                 "." ;
    constexpr string_view expected = "digraph {\n"
                                     "\tFunction -> Return Statement;\n"
                                     "\tReturn Statement -> Literal;\n"
                                     "\tLiteral -> \"1\";\n"
                                     "}\n" ;
    CodeManager manager(code) ;
    TokenStream tokenStream(&manager) ;
    tokenStream.compileCode() ;
    ASSERT_TRUE(!manager.isCodeError()) ;
    FunctionDeclaration functionDeclaration(&manager) ;
    ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
    ASSERT_TRUE(!manager.isCodeError()) ;
    FunctionAST functionAst(&manager) ;
    ASSERT_TRUE(functionAst.compileCode(functionDeclaration)) ;
    ASSERT_TRUE(!manager.isCodeError()) ;
    TestPrintASTVisitor printAstVisitor ;
    functionAst.accept(printAstVisitor) ;
    ASSERT_EQ(expected , printAstVisitor.getOutput()) ;
}
TEST(TestAST , TestIdentifier)
{
    {
        constexpr string_view code = "PARAM a ;\n"
                                     "BEGIN\n"
                                     "RETURN a\n"
                                     "END\n"
                                     ".\n" ;
        constexpr string_view expected = "digraph {\n"
                                         "\tFunction -> Return Statement;\n"
                                         "\tReturn Statement -> Identifier;\n"
                                         "\tIdentifier -> \"a\";\n"
                                         "}\n" ;
        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        FunctionAST functionAst(&manager) ;
        ASSERT_TRUE(functionAst.compileCode(functionDeclaration)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintASTVisitor printAstVisitor ;
        functionAst.accept(printAstVisitor) ;
        ASSERT_EQ(expected , printAstVisitor.getOutput()) ;


    }
    {
        // undeclared identifier->{0,2} , uninitialized variable -> {1 , 3}
        constexpr array<string_view , 4> functions = {
            // undeclared
            "PARAM a;\n"
            "BEGIN\n"
            "RETURN b\n"
            "END\n"
            "."
            ,
            // uninitialized VAR
            "VAR a;\n"
            "BEGIN\n"
            "RETURN a\n"
            "END\n"
            ".\n"
            ,
            // undeclared
            "PARAM a;\n"
            "VAR b;\n"
            "BEGIN\n"
            "b := c + a;\n"
            "RETURN b\n"
            "END\n"
            ".\n"
            ,
            // uninitialized VAR
            "PARAM a;\n"
            "VAR b;\n"
            "BEGIN\n"
            "b := b + a;\n"
            "RETURN b\n"
            "END\n"
            ".\n"

        };
        constexpr array<string_view , 4 > expectedMessages = {
            "3:8: error: Undeclared Identifier\n"
            "RETURN b\n"
            "       ^\n" ,
            "3:8: error: Uninitialized Identifier\n"
            "RETURN a\n"
            "       ^\n" ,
            "4:6: error: Undeclared Identifier\n"
            "b := c + a;\n"
            "     ^\n" ,
            "4:6: error: Uninitialized Identifier\n"
            "b := b + a;\n"
            "     ^\n"
        };
        for(size_t index = 0 ; index < 4 ; index ++) {
            const auto& code = functions[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(code) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            FunctionDeclaration functionDeclaration(&manager) ;
            ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            FunctionAST functionAst(&manager) ;
            ASSERT_TRUE(!functionAst.compileCode(functionDeclaration)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;

        }

    }
}
TEST(TestAST , TestUnaryExpression)
{
    {
        constexpr array<string_view, 2> functions = {
            "PARAM a ;\n"
            "VAR b;\n"
            "BEGIN\n"
            "b := -a + 10;\n"
            "RETURN +b\n"
            "END\n"
            ".\n" ,
            "PARAM a ;\n"
            "VAR b;\n"
            "BEGIN\n"
            "b := +a + 10;\n"
            "RETURN -b\n"
            "END\n"
            ".\n"
        };
        const array<string_view , 2> expectedExpression = {
            "digraph {\n"
            "\tFunction -> Assignment Statement(:=);\n"
            "\tFunction -> Return Statement;\n"
            "\tAssignment Statement(:=) -> Identifier;\n"
            "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
            "\tBinaryExpression(+)-> UnaryExpression(-);\n"
            "\tBinaryExpression(+) -> Literal;\n"
            "\tUnaryExpression(-) -> Identifier;\n"
            "\tIdentifier -> \"a\";\n"
            "\tLiteral -> \"10\";\n"
            "\tReturn Statement-> UnaryExpression(+);\n"
            "\tUnaryExpression(+) -> Identifier;\n"
            "\tIdentifier -> \"b\";\n"
            "}\n"
            ,
            "digraph {\n"
            "\tFunction -> Assignment Statement(:=);\n"
            "\tFunction -> Return Statement;\n"
            "\tAssignment Statement(:=) -> Identifier;\n"
            "\tAssignment Statement(:=) -> BinaryExpression(+);\n"
            "\tBinaryExpression(+)-> UnaryExpression(+);\n"
            "\tBinaryExpression(+) -> Literal;\n"
            "\tUnaryExpression(+) -> Identifier;\n"
            "\tIdentifier -> \"a\";\n"
            "\tLiteral -> \"10\";\n"
            "\tReturn Statement-> UnaryExpression(-);\n"
            "\tUnaryExpression(-) -> Identifier;\n"
            "\tIdentifier -> \"b\";\n"
            "}\n"
        };
        for(size_t index = 0 ; index < 2 ; index ++) {
            const auto& code = functions[index] ;
            const auto& expected = expectedExpression[index] ;
            CodeManager manager(code) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            FunctionDeclaration functionDeclaration(&manager) ;
            ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            FunctionAST functionAst(&manager) ;
            ASSERT_TRUE(functionAst.compileCode(functionDeclaration)) ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            TestPrintASTVisitor printAstVisitor ;
            functionAst.accept(printAstVisitor) ;
            ASSERT_EQ(expected , printAstVisitor.getOutput()) ;
        }

    }
    {
        constexpr array<string_view , 4> functions = {
            // undeclared
            "PARAM a;\n"
            "BEGIN\n"
            "RETURN +b\n"
            "END\n"
            "."
            ,
            // uninitialized VAR
            "VAR a;\n"
            "BEGIN\n"
            "RETURN -a\n"
            "END\n"
            ".\n"
            ,
            // undeclared
            "PARAM a;\n"
            "VAR b;\n"
            "BEGIN\n"
            "b := -c + a;\n"
            "RETURN b\n"
            "END\n"
            ".\n"
            ,
            // uninitialized VAR
            "PARAM a;\n"
            "VAR b;\n"
            "BEGIN\n"
            "b := +b + a;\n"
            "RETURN b\n"
            "END\n"
            ".\n"

        };
        constexpr array<string_view , 4 > expectedMessages = {
            "3:9: error: Undeclared Identifier\n"
            "RETURN +b\n"
            "        ^\n" ,
            "3:9: error: Uninitialized Identifier\n"
            "RETURN -a\n"
            "        ^\n" ,
            "4:7: error: Undeclared Identifier\n"
            "b := -c + a;\n"
            "      ^\n" ,
            "4:7: error: Uninitialized Identifier\n"
            "b := +b + a;\n"
            "      ^\n"
        };
        for(size_t index = 0 ; index < 4 ; index ++) {
            const auto& code = functions[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(code) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            FunctionDeclaration functionDeclaration(&manager) ;
            ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
            ASSERT_TRUE(!manager.isCodeError()) ;
            FunctionAST functionAst(&manager) ;
            ASSERT_TRUE(!functionAst.compileCode(functionDeclaration)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;

        }
    }
}

TEST(TestAST , TestBinaryExpression)
{
    {
        constexpr array<string_view, 4> functions = {
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a + b\n"
            "END\n"
            ".\n",
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a - b\n"
            "END\n"
            ".\n",
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a * b\n"
            "END\n"
            ".\n",
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a / b\n"
            "END\n"
            ".\n"};
        constexpr array<string_view, 4> expectedExpression = {
            "digraph {\n"
            "\tFunction -> Return Statement;\n"
            "\tReturn Statement -> BinaryExpression(+);\n"
            "\tBinaryExpression(+) -> Identifier;\n"
            "\tBinaryExpression(+) -> Identifier;\n"
            "\tIdentifier -> \"a\";\n"
            "\tIdentifier -> \"b\";\n"
            "}\n",
            "digraph {\n"
            "\tFunction -> Return Statement;\n"
            "\tReturn Statement -> BinaryExpression(-);\n"
            "\tBinaryExpression(-) -> Identifier;\n"
            "\tBinaryExpression(-) -> Identifier;\n"
            "\tIdentifier -> \"a\";\n"
            "\tIdentifier -> \"b\";\n"
            "}\n",
            "digraph {\n"
            "\tFunction -> Return Statement;\n"
            "\tReturn Statement -> BinaryExpression(*);\n"
            "\tBinaryExpression(*) -> Identifier;\n"
            "\tBinaryExpression(*) -> Identifier;\n"
            "\tIdentifier -> \"a\";\n"
            "\tIdentifier -> \"b\";\n"
            "}\n",
            "digraph {\n"
            "\tFunction -> Return Statement;\n"
            "\tReturn Statement -> BinaryExpression(/);\n"
            "\tBinaryExpression(/) -> Identifier;\n"
            "\tBinaryExpression(/) -> Identifier;\n"
            "\tIdentifier -> \"a\";\n"
            "\tIdentifier -> \"b\";\n"
            "}\n"};
        for (size_t index = 0; index < 4; index++) {
            const auto& code = functions[index];
            const auto& expected = expectedExpression[index];
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
            TestPrintASTVisitor printAstVisitor;
            functionAst.accept(printAstVisitor);
            ASSERT_EQ(expected, printAstVisitor.getOutput());
        }
    }
    {
        constexpr string_view code =
            "VAR a , b;\n"
            "BEGIN\n"
            "RETURN a + b\n"
            "END\n"
            ".\n" ;
        constexpr string_view expected = "3:8: error: Uninitialized Identifier\n"
            "RETURN a + b\n"
            "       ^\n" ;
        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        FunctionAST functionAst(&manager) ;
        ASSERT_TRUE(!functionAst.compileCode(functionDeclaration)) ;
        ASSERT_TRUE(manager.isCodeError()) ;
        ASSERT_EQ(expected , manager.error_message()) ;
    }
}
TEST(TestAST , TestFunction)
{
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
        constexpr string_view expected = "digraph {\n"
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
        TestPrintASTVisitor printAstVisitor;
        functionAst.accept(printAstVisitor);
        ASSERT_EQ(expected, printAstVisitor.getOutput());
    }
    {
        constexpr string_view code =
            "PARAM xa , xb , ya , yb;\n"
            "VAR x , y , d;\n"
            "BEGIN\n"
            "x := (xa - xb) * (xa - xb);\n"
            "y := (ya - yb) * (ya - yb);\n"
            "d := x + y\n"
            "END.\n" ;
        constexpr string_view expected = "7:1: error: Missing Return Statement\n"
                                         "END.\n"
                                         "^~~\n" ;

        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        FunctionAST functionAst(&manager) ;
        ASSERT_TRUE(!functionAst.compileCode(functionDeclaration)) ;
        ASSERT_TRUE(manager.isCodeError()) ;
        ASSERT_EQ(expected , manager.error_message()) ;

    }
}

