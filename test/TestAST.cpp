#include <gtest/gtest.h>

#include "pljit/CodeManager.hpp"
#include "pljit/TokenStream.hpp"
#include "pljit/ParseTree.hpp"
#include "pljit/AST.hpp"

using namespace std ;
using namespace jitcompiler ;

TEST(TestAST , TestSymbolTable)
{
    // TODO check for multiple declaration of the same identifier
    {
        constexpr string_view code = "PARAM a , b , c;"
                                     "VAR d , e , f;"
                                     "CONST g=1,h=2,i=3;"
                                     "BEGIN"
                                     "RETURN 0"
                                     "END"
                                     "." ;

        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        SymbolTable symbolTable(functionDeclaration) ;
        ASSERT_TRUE(symbolTable.isComplied()) ;
    }
    {
        constexpr string_view code = "PARAM a , b , c;"
                                     "VAR a , e , f;"
                                     "CONST g=1,h=2,i=3;"
                                     "BEGIN"
                                     "RETURN 0"
                                     "END"
                                     "." ;
        CodeManager manager(code) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        FunctionDeclaration functionDeclaration(&manager) ;
        ASSERT_TRUE(functionDeclaration.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        SymbolTable symbolTable(functionDeclaration) ;
        ASSERT_TRUE(!symbolTable.isComplied()) ;
        // TODO errorMessage
        // ASSERT_EQ() ;
    }

}

TEST(TestAST , TestLiteral) {
    constexpr string_view code = "BEGIN"
                                 "RETURN 1"
                                 "END"
                                 "." ;

}
TEST(TestAST , TestIdentifier)
{

    {
        constexpr string_view code = "PARAM a ;"
                                     "BEGIN"
                                     "RETURN a"
                                     "END"
                                     "." ;

    }

    {
        constexpr string_view code1 = "PARAM a;"
                                     "BEGIN"
                                     "RETURN b" // 2
                                     "END"
                                     "." ;
        constexpr string_view code2 = "VAR a;"
                                     "BEGIN"
                                     "RETURN a" // 4
                                     "END"
                                     "." ;

    }
}
TEST(TestAST , TestUnaryExpression)
{
    {
        constexpr string_view code1 = "PARAM a ;"
                                     "BEGIN"
                                     "RETURN +a"
                                     "END"
                                     "." ;
        constexpr string_view code2 = "PARAM a ;"
                                     "BEGIN"
                                     "RETURN -10"
                                     "END"
                                     "." ;
    }
    {
        
    }
}

TEST(TestAST , TestBinaryExpression)
{

}
TEST(TestAST , TestAssignmentStatement)
{

}

TEST(TestAST , TestReturnStatement)
{

}

TEST(TestAST , TestFunction)
{

}

