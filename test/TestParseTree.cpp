#include "pljit/PrintParseTreeVisitor.hpp"
#include "pljit/ParseTree.hpp"
#include <gtest/gtest.h>

using namespace std ;
using namespace jitcompiler ;
namespace {
    using TestPrintVisitor = PrintVisitor<false>;
}//namespace

TEST(TestParseTree , PrimaryExpressionWithoutBrackets){
    {
        string identifier = "volume";
        string expectedDot = "digraph {\n"
                             "\tprimary-expression -> identifier;\n"
                             "\tidentifier -> \"volume\";\n"
                             "}\n" ;
        CodeManager manager(identifier) ;
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode() ;
        PrimaryExpression primaryExpression(&manager) ;
        ASSERT_TRUE(primaryExpression.recursiveDecentParser(lexicalAnalyzer)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintVisitor printVisitor ;
        primaryExpression.accept(printVisitor) ;
        ASSERT_EQ(expectedDot , printVisitor.getOutput()) ;
    }

    {
        string literal = "1213";
        string expectedDot = "digraph {\n"
                             "\tprimary-expression -> literal;\n"
                             "\tliteral -> \"1213\";\n"
                             "}\n" ;
        CodeManager manager(literal) ;
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode() ;
        PrimaryExpression primaryExpression(&manager) ;
        ASSERT_TRUE(primaryExpression.recursiveDecentParser(lexicalAnalyzer)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintVisitor printVisitor ;
        primaryExpression.accept(printVisitor) ;
        ASSERT_EQ(expectedDot , printVisitor.getOutput()) ;
    }
}
TEST(TestParseTree , UnaryExpressionExpressionWithoutBrackets){
    {
        string identifier = "volume";
        string expectedDot = "digraph {\n"
                             "\tunary-expression -> primary-expression;\n"
                             "\tprimary-expression -> identifier;\n"
                             "\tidentifier -> \"volume\";\n"
                             "}\n" ;
        CodeManager manager(identifier) ;
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode() ;
        UnaryExpression unaryExpression(&manager) ;
        ASSERT_TRUE(unaryExpression.recursiveDecentParser(lexicalAnalyzer)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintVisitor printVisitor ;
        unaryExpression.accept(printVisitor) ;
        ASSERT_EQ(expectedDot , printVisitor.getOutput()) ;
    }
    {
        string literal = "1213";
        string expectedDot = "digraph {\n"
                             "\tunary-expression -> primary-expression;\n"
                             "\tprimary-expression -> literal;\n"
                             "\tliteral -> \"1213\";\n"
                             "}\n" ;
        CodeManager manager(literal) ;
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode() ;
        UnaryExpression unaryExpression(&manager) ;
        ASSERT_TRUE(unaryExpression.recursiveDecentParser(lexicalAnalyzer)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintVisitor printVisitor ;
        unaryExpression.accept(printVisitor) ;
        ASSERT_EQ(expectedDot , printVisitor.getOutput()) ;
    }
}
