#include "pljit/syntax/ParseTree.hpp"
#include "pljit/syntax/PrintParseTreeVisitor.hpp"
#include <gtest/gtest.h>

using namespace std ;
using namespace jitcompiler ;
using namespace jitcompiler ::management;
using namespace jitcompiler ::syntax;

TEST(TestParseTree , TestLiteral) {
    { // feasible to compile
        constexpr string_view token = "1" ;
        constexpr string_view expected = "digraph {\n"
                                         "\tliteral -> \"1\";\n"
                                         "}\n" ;
        CodeManager manager(token) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        Literal literal(&manager) ;
        ASSERT_TRUE(literal.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintVisitor printVisitor;
        literal.accept(printVisitor) ;
        ASSERT_EQ(expected , printVisitor.getOutput()) ;
    }
    { // not feasible to compile
        constexpr array<string_view , 2> tokens = {"" , "id"} ;
        constexpr array<string_view , 2>  expectedMessage = {
            "1:1: error: expected \"Literal Token\"\n"
            "\n"
            "^" ,
            "1:1: error: expected Literal Token\n"
            "id\n"
            "^~\n"
        };
        for(size_t index = 0 ; index < 2 ; index ++) {
            const auto& token = tokens[index] ;
            const auto& expected = expectedMessage[index] ;
            CodeManager manager(token) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            Literal literal(&manager) ;
            ASSERT_TRUE(!literal.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }
}

TEST(TestParseTree , TestIdentifier) {
    { // feasible to compile

        constexpr string_view token = "index"  ;
        constexpr string_view expected = "digraph {\n"
                                         "\tidentifier -> \"index\";\n"
                                         "}\n" ;
        CodeManager manager(token) ;
        TokenStream tokenStream(&manager) ;
        tokenStream.compileCode() ;
        Identifier identifier(&manager) ;
        ASSERT_TRUE(identifier.compileCode(tokenStream)) ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        TestPrintVisitor printVisitor ;
        identifier.accept(printVisitor) ;
        ASSERT_EQ(expected , printVisitor.getOutput()) ;
    }
    { // not feasible to compile
        constexpr array<string_view , 2> tokens = {"" , "PARAM"} ;
        constexpr array<string_view , 2> expectedMessage = {
            "1:1: error: expected \"Identifier Token\"\n"
            "\n"
            "^" ,
            "1:1: error: expected Identifier Token\n"
            "PARAM\n"
            "^~~~~\n"
        };
        for(size_t index = 0 ; index < 2 ; index ++)
        {
            const auto& token = tokens[index] ;
            const auto& expected = expectedMessage[index] ;
            CodeManager manager(token) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            Identifier identifier(&manager) ;
            ASSERT_TRUE(!identifier.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }
}

TEST(TestParseTree , TestPrimaryExpression) {

    { // feasible to compile
        constexpr array<string_view, 4> terminalExpression = {"volume", "1213", "(volume)", "(1213)"};
        constexpr array<string_view, 4> expectedPrimary = {"digraph {\n"
                                                           "\tprimary-expression -> identifier;\n"
                                                           "\tidentifier -> \"volume\";\n"
                                                           "}\n",

                                                           "digraph {\n"
                                                           "\tprimary-expression -> literal;\n"
                                                           "\tliteral -> \"1213\";\n"
                                                           "}\n",
                                                           "digraph {\n"
                                                           "\tprimary-expression -> (;\n"
                                                           "\tprimary-expression -> additive-expression;\n"
                                                           "\tprimary-expression -> );\n"
                                                           "\tadditive-expression -> multiplicative-expression;\n"
                                                           "\tmultiplicative-expression -> unary-expression;\n"
                                                           "\tunary-expression -> primary-expression;\n"
                                                           "\tprimary-expression -> identifier;\n"
                                                           "\tidentifier -> \"volume\";\n"
                                                           "}\n",
                                                           "digraph {\n"
                                                           "\tprimary-expression -> (;\n"
                                                           "\tprimary-expression -> additive-expression;\n"
                                                           "\tprimary-expression -> );\n"
                                                           "\tadditive-expression -> multiplicative-expression;\n"
                                                           "\tmultiplicative-expression -> unary-expression;\n"
                                                           "\tunary-expression -> primary-expression;\n"
                                                           "\tprimary-expression -> literal;\n"
                                                           "\tliteral -> \"1213\";\n"
                                                           "}\n"};
        constexpr array<string_view, 4> expectedUnary = {"digraph {\n"
                                                         "\tunary-expression -> primary-expression;\n"
                                                         "\tprimary-expression -> identifier;\n"
                                                         "\tidentifier -> \"volume\";\n"
                                                         "}\n",
                                                         "digraph {\n"
                                                         "\tunary-expression -> primary-expression;\n"
                                                         "\tprimary-expression -> literal;\n"
                                                         "\tliteral -> \"1213\";\n"
                                                         "}\n",
                                                         "digraph {\n"
                                                         "\tunary-expression -> primary-expression;\n"
                                                         "\tprimary-expression -> (;\n"
                                                         "\tprimary-expression -> additive-expression;\n"
                                                         "\tprimary-expression -> );\n"
                                                         "\tadditive-expression -> multiplicative-expression;\n"
                                                         "\tmultiplicative-expression -> unary-expression;\n"
                                                         "\tunary-expression -> primary-expression;\n"
                                                         "\tprimary-expression -> identifier;\n"
                                                         "\tidentifier -> \"volume\";\n"
                                                         "}\n",
                                                         "digraph {\n"
                                                         "\tunary-expression -> primary-expression;\n"
                                                         "\tprimary-expression -> (;\n"
                                                         "\tprimary-expression -> additive-expression;\n"
                                                         "\tprimary-expression -> );\n"
                                                         "\tadditive-expression -> multiplicative-expression;\n"
                                                         "\tmultiplicative-expression -> unary-expression;\n"
                                                         "\tunary-expression -> primary-expression;\n"
                                                         "\tprimary-expression -> literal;\n"
                                                         "\tliteral -> \"1213\";\n"
                                                          "}\n"};
        constexpr array<string_view, 4> expectedMult = {"digraph {\n"
                                                        "\tmultiplicative-expression -> unary-expression;\n"
                                                        "\tunary-expression -> primary-expression;\n"
                                                        "\tprimary-expression -> identifier;\n"
                                                        "\tidentifier -> \"volume\";\n"
                                                        "}\n",

                                                        "digraph {\n"
                                                        "\tmultiplicative-expression -> unary-expression;\n"
                                                        "\tunary-expression -> primary-expression;\n"
                                                        "\tprimary-expression -> literal;\n"
                                                        "\tliteral -> \"1213\";\n"
                                                        "}\n",
                                                        "digraph {\n"
                                                        "\tmultiplicative-expression -> unary-expression;\n"
                                                        "\tunary-expression -> primary-expression;\n"
                                                        "\tprimary-expression -> (;\n"
                                                        "\tprimary-expression -> additive-expression;\n"
                                                        "\tprimary-expression -> );\n"
                                                        "\tadditive-expression -> multiplicative-expression;\n"
                                                        "\tmultiplicative-expression -> unary-expression;\n"
                                                        "\tunary-expression -> primary-expression;\n"
                                                        "\tprimary-expression -> identifier;\n"
                                                        "\tidentifier -> \"volume\";\n"
                                                        "}\n",
                                                        "digraph {\n"
                                                        "\tmultiplicative-expression -> unary-expression;\n"
                                                        "\tunary-expression -> primary-expression;\n"
                                                        "\tprimary-expression -> (;\n"
                                                        "\tprimary-expression -> additive-expression;\n"
                                                        "\tprimary-expression -> );\n"
                                                        "\tadditive-expression -> multiplicative-expression;\n"
                                                        "\tmultiplicative-expression -> unary-expression;\n"
                                                        "\tunary-expression -> primary-expression;\n"
                                                        "\tprimary-expression -> literal;\n"
                                                        "\tliteral -> \"1213\";\n"
                                                        "}\n"};
        constexpr array<string_view, 4> expectedAdd = {"digraph {\n"
                                                       "\tadditive-expression -> multiplicative-expression;\n"
                                                       "\tmultiplicative-expression -> unary-expression;\n"
                                                       "\tunary-expression -> primary-expression;\n"
                                                       "\tprimary-expression -> identifier;\n"
                                                       "\tidentifier -> \"volume\";\n"
                                                       "}\n",

                                                       "digraph {\n"
                                                       "\tadditive-expression -> multiplicative-expression;\n"
                                                       "\tmultiplicative-expression -> unary-expression;\n"
                                                       "\tunary-expression -> primary-expression;\n"
                                                       "\tprimary-expression -> literal;\n"
                                                       "\tliteral -> \"1213\";\n"
                                                       "}\n",
                                                       "digraph {\n"
                                                       "\tadditive-expression -> multiplicative-expression;\n"
                                                       "\tmultiplicative-expression -> unary-expression;\n"
                                                       "\tunary-expression -> primary-expression;\n"
                                                       "\tprimary-expression -> (;\n"
                                                       "\tprimary-expression -> additive-expression;\n"
                                                       "\tprimary-expression -> );\n"
                                                       "\tadditive-expression -> multiplicative-expression;\n"
                                                       "\tmultiplicative-expression -> unary-expression;\n"
                                                       "\tunary-expression -> primary-expression;\n"
                                                       "\tprimary-expression -> identifier;\n"
                                                       "\tidentifier -> \"volume\";\n"
                                                       "}\n",
                                                       "digraph {\n"
                                                       "\tadditive-expression -> multiplicative-expression;\n"
                                                       "\tmultiplicative-expression -> unary-expression;\n"
                                                       "\tunary-expression -> primary-expression;\n"
                                                       "\tprimary-expression -> (;\n"
                                                       "\tprimary-expression -> additive-expression;\n"
                                                       "\tprimary-expression -> );\n"
                                                       "\tadditive-expression -> multiplicative-expression;\n"
                                                       "\tmultiplicative-expression -> unary-expression;\n"
                                                       "\tunary-expression -> primary-expression;\n"
                                                       "\tprimary-expression -> literal;\n"
                                                       "\tliteral -> \"1213\";\n"
                                                       "}\n"};
        for (size_t indexPrimary = 0; indexPrimary < 4; indexPrimary++) {
            const auto& cur = terminalExpression[indexPrimary];
            const auto& expected = expectedPrimary[indexPrimary];
            CodeManager manager(cur);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            PrimaryExpression primaryExpression(&manager);
            ASSERT_TRUE(primaryExpression.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            primaryExpression.accept(printVisitor);
            ASSERT_EQ(expected, printVisitor.getOutput());
        }
        for (size_t indexUnary = 0; indexUnary < 4; indexUnary++) {
            const auto& cur = terminalExpression[indexUnary];
            const auto& expected = expectedUnary[indexUnary];
            CodeManager manager(cur);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            UnaryExpression unaryExpression(&manager);
            ASSERT_TRUE(unaryExpression.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            unaryExpression.accept(printVisitor);
            ASSERT_EQ(expected, printVisitor.getOutput());
        }
        for (size_t indexMult = 0; indexMult < 4; indexMult++) {
            const auto& cur = terminalExpression[indexMult];
            const auto& expected = expectedMult[indexMult];
            CodeManager manager(cur);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            MultiplicativeExpression primaryExpression(&manager);
            ASSERT_TRUE(primaryExpression.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            primaryExpression.accept(printVisitor);
            ASSERT_EQ(expected, printVisitor.getOutput());
        }
        for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
            const auto& cur = terminalExpression[indexAdd];
            const auto& expected = expectedAdd[indexAdd];
            CodeManager manager(cur);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            AdditiveExpression primaryExpression(&manager);
            ASSERT_TRUE(primaryExpression.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            primaryExpression.accept(printVisitor);
            ASSERT_EQ(expected, printVisitor.getOutput());
        }
    }
    { // not feasible to compile
        constexpr array<string_view , 4> expressions = {"" , "(VAR + 5)" , "((a + b) * c" , "(a + b)) * c"};
        constexpr array<string_view , 4> expectedMessages = {
            "1:1: error: expected \"Identifier , Literal or Open Bracket\"\n"
            "\n"
            "^" ,
            "1:2: error: expected Identifier , Literal or Open Bracket\n"
            "(VAR + 5)\n"
            " ^~~\n" ,
            "1:13: error: expected \")\"\n"
            "((a + b) * c\n"
            "            ^" ,
            "1:8: error: unexpected token \")\"\n"
            "(a + b)) * c\n"
            "       ^\n"
        };
        for(size_t index = 0 ; index < 4 ; index ++) {
            const auto& expression = expressions[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(expression) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            PrimaryExpression identifier(&manager) ;
            ASSERT_TRUE(!identifier.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }

    }
}

TEST(TestParseTree , TestUnaryExpression) {
    constexpr array<string_view , 4> unaryExpressions = {"+volume", "(-volume)", "(+1213)", "-1213"};
    constexpr array<string_view , 4> expectedUnary = {"digraph {\n"
                                 "\tunary-expression -> +;\n"
                                 "\tunary-expression -> primary-expression;\n"
                                 "\tprimary-expression -> identifier;\n"
                                 "\tidentifier -> \"volume\";\n"
                                 "}\n",
                                 "digraph {\n"
                                 "\tunary-expression -> primary-expression;\n"
                                 "\tprimary-expression -> (;\n"
                                 "\tprimary-expression -> additive-expression;\n"
                                 "\tprimary-expression -> );\n"
                                 "\tadditive-expression -> multiplicative-expression;\n"
                                 "\tmultiplicative-expression -> unary-expression;\n"
                                 "\tunary-expression -> -;\n"
                                 "\tunary-expression -> primary-expression;\n"
                                 "\tprimary-expression -> identifier;\n"
                                 "\tidentifier -> \"volume\";\n"
                                 "}\n",
                                 "digraph {\n"
                                 "\tunary-expression -> primary-expression;\n"
                                 "\tprimary-expression -> (;\n"
                                 "\tprimary-expression -> additive-expression;\n"
                                 "\tprimary-expression -> );\n"
                                 "\tadditive-expression -> multiplicative-expression;\n"
                                 "\tmultiplicative-expression -> unary-expression;\n"
                                 "\tunary-expression -> +;\n"
                                 "\tunary-expression -> primary-expression;\n"
                                 "\tprimary-expression -> literal;\n"
                                 "\tliteral -> \"1213\";\n"
                                 "}\n" ,
                                 "digraph {\n"
                                 "\tunary-expression -> -;\n"
                                 "\tunary-expression -> primary-expression;\n"
                                 "\tprimary-expression -> literal;\n"
                                 "\tliteral -> \"1213\";\n"
                                 "}\n"

    };
    constexpr array<string_view , 4> expectedMult = {
                                "digraph {\n"
                                 "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> +;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> identifier;\n"
                                "\tidentifier -> \"volume\";\n"
                                "}\n",
                                "digraph {\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> (;\n"
                                "\tprimary-expression -> additive-expression;\n"
                                "\tprimary-expression -> );\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> -;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> identifier;\n"
                                "\tidentifier -> \"volume\";\n"
                                "}\n",
                                "digraph {\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> (;\n"
                                "\tprimary-expression -> additive-expression;\n"
                                "\tprimary-expression -> );\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> +;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> literal;\n"
                                "\tliteral -> \"1213\";\n"
                                "}\n" ,
                                "digraph {\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> -;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> literal;\n"
                                "\tliteral -> \"1213\";\n"
                                "}\n"

    };
    constexpr array<string_view , 4> expectedAdd = {
                                "digraph {\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> +;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> identifier;\n"
                                "\tidentifier -> \"volume\";\n"
                                "}\n",
                                "digraph {\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> (;\n"
                                "\tprimary-expression -> additive-expression;\n"
                                "\tprimary-expression -> );\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> -;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> identifier;\n"
                                "\tidentifier -> \"volume\";\n"
                                "}\n",
                                "digraph {\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> (;\n"
                                "\tprimary-expression -> additive-expression;\n"
                                "\tprimary-expression -> );\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> +;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> literal;\n"
                                "\tliteral -> \"1213\";\n"
                                "}\n" ,
                                "digraph {\n"
                                "\tadditive-expression -> multiplicative-expression;\n"
                                "\tmultiplicative-expression -> unary-expression;\n"
                                "\tunary-expression -> -;\n"
                                "\tunary-expression -> primary-expression;\n"
                                "\tprimary-expression -> literal;\n"
                                "\tliteral -> \"1213\";\n"
                                "}\n"

    };

    for (size_t indexUnary = 0; indexUnary < 4; indexUnary++) {
        const auto& expression = unaryExpressions[indexUnary];
        const auto& expectedDot = expectedUnary[indexUnary];
        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        UnaryExpression unaryExpression(&manager);
        ASSERT_TRUE(unaryExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        unaryExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }
    for (size_t indexMult = 0; indexMult < 4; indexMult++) {
        const auto& expression = unaryExpressions[indexMult];
        const auto& expectedDot = expectedMult[indexMult];
        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        MultiplicativeExpression multiplicativeExpression(&manager);
        ASSERT_TRUE(multiplicativeExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        multiplicativeExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }
    for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
        const auto& expression = unaryExpressions[indexAdd];
        const auto& expectedDot = expectedAdd[indexAdd];
        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AdditiveExpression additiveExpression(&manager);
        ASSERT_TRUE(additiveExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        additiveExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }

}

TEST(TestParseTree , TestMultExpression) {
    constexpr array<string_view , 4> multExpressions = {"a*b", "-10 /5", "( a* + 12)" , "(a/b)"};
    constexpr array<string_view , 4> expectedMult = {
        "digraph {\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> *;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"b\";\n"
        "}\n",
        "digraph {\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> /;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> -;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"10\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"5\";\n"
        "}\n" ,
        "digraph {\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> (;\n"
        "\tprimary-expression -> additive-expression;\n"
        "\tprimary-expression -> );\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> *;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> +;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"12\";\n"
        "}\n"
        ,
        "digraph {\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> (;\n"
        "\tprimary-expression -> additive-expression;\n"
        "\tprimary-expression -> );\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> /;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"b\";\n"
        "}\n"
    };
    constexpr array<string_view , 4> expectedAdd = {
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> *;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"b\";\n"
        "}\n",
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> /;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> -;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"10\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"5\";\n"
        "}\n" ,
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> (;\n"
        "\tprimary-expression -> additive-expression;\n"
        "\tprimary-expression -> );\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> *;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> +;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"12\";\n"
        "}\n"
        ,
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> (;\n"
        "\tprimary-expression -> additive-expression;\n"
        "\tprimary-expression -> );\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> /;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"b\";\n"
        "}\n"
    };

    for (size_t indexMult = 0; indexMult < 4; indexMult++) {
        const auto& expression = multExpressions[indexMult];
        const auto& expectedDot = expectedMult[indexMult];

        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        MultiplicativeExpression multiplicativeExpression(&manager);
        ASSERT_TRUE(multiplicativeExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        multiplicativeExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }
    for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
        const auto& expression = multExpressions[indexAdd];
        const auto& expectedDot = expectedAdd[indexAdd];

        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AdditiveExpression additiveExpression(&manager);
        ASSERT_TRUE(additiveExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        additiveExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree, TestAddExpression) {
    constexpr array<string_view , 4> expressions = { "12+ 5", "(a -5)", "-5 + 10* a", "+a * (3 + - b)"};
    constexpr array<string_view , 4> expectedAdd = {
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tadditive-expression -> +;\n"
        "\tadditive-expression -> additive-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"12\";\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"5\";\n"
        "}\n",
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> (;\n"
        "\tprimary-expression -> additive-expression;\n"
        "\tprimary-expression -> );\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tadditive-expression -> -;\n"
        "\tadditive-expression -> additive-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"5\";\n"
        "}\n",
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tadditive-expression -> +;\n"
        "\tadditive-expression -> additive-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> -;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"5\";\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> *;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"10\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "}\n",
        "digraph {\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tmultiplicative-expression -> *;\n"
        "\tmultiplicative-expression -> multiplicative-expression;\n"
        "\tunary-expression -> +;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"a\";\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> (;\n"
        "\tprimary-expression -> additive-expression;\n"
        "\tprimary-expression -> );\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tadditive-expression -> +;\n"
        "\tadditive-expression -> additive-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> literal;\n"
        "\tliteral -> \"3\";\n"
        "\tadditive-expression -> multiplicative-expression;\n"
        "\tmultiplicative-expression -> unary-expression;\n"
        "\tunary-expression -> -;\n"
        "\tunary-expression -> primary-expression;\n"
        "\tprimary-expression -> identifier;\n"
        "\tidentifier -> \"b\";\n"
        "}\n"
    };
    constexpr string_view expectedPrimary = "digraph {\n"
                             "\tprimary-expression -> (;\n"
                             "\tprimary-expression -> additive-expression;\n"
                             "\tprimary-expression -> );\n"
                             "\tadditive-expression -> multiplicative-expression;\n"
                             "\tadditive-expression -> -;\n"
                             "\tadditive-expression -> additive-expression;\n"
                             "\tmultiplicative-expression -> unary-expression;\n"
                             "\tunary-expression -> primary-expression;\n"
                             "\tprimary-expression -> identifier;\n"
                             "\tidentifier -> \"a\";\n"
                             "\tadditive-expression -> multiplicative-expression;\n"
                             "\tmultiplicative-expression -> unary-expression;\n"
                             "\tunary-expression -> primary-expression;\n"
                             "\tprimary-expression -> literal;\n"
                             "\tliteral -> \"5\";\n"
                             "}\n";

    for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
        const auto& expression = expressions[indexAdd];
        const auto& expectedDot = expectedAdd[indexAdd];

        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AdditiveExpression additiveExpression(&manager);
        ASSERT_TRUE(additiveExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        additiveExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }
    { // Test primary-expression := "(" additive-expression ")"
        const auto& expression = expressions[1];
        const auto& expectedDot = expectedPrimary;

        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        PrimaryExpression primaryExpression(&manager);
        ASSERT_TRUE(primaryExpression.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        primaryExpression.accept(printVisitor);
        ASSERT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestAssignmentExpression) {
    {
        constexpr array<string_view, 2> assignmentStatements = {"volume := 1234", "volume :=  w * h * d"};
        constexpr array<string_view, 2> expected = {"digraph {\n"
                                                    "\tassignment-expression -> identifier;\n"
                                                    "\tassignment-expression -> :=;\n"
                                                    "\tassignment-expression -> additive-expression;\n"
                                                    "\tidentifier -> \"volume\";\n"
                                                    "\tadditive-expression -> multiplicative-expression;\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> literal;\n"
                                                    "\tliteral -> \"1234\";\n"
                                                    "}\n",
                                                    "digraph {\n"
                                                    "\tassignment-expression -> identifier;\n"
                                                    "\tassignment-expression -> :=;\n"
                                                    "\tassignment-expression -> additive-expression;\n"
                                                    "\tidentifier -> \"volume\";\n"
                                                    "\tadditive-expression -> multiplicative-expression;\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tmultiplicative-expression -> *;\n"
                                                    "\tmultiplicative-expression -> multiplicative-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> identifier;\n"
                                                    "\tidentifier -> \"w\";\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tmultiplicative-expression -> *;\n"
                                                    "\tmultiplicative-expression -> multiplicative-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> identifier;\n"
                                                    "\tidentifier -> \"h\";\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> identifier;\n"
                                                    "\tidentifier -> \"d\";\n"
                                                    "}\n"};
        for (size_t index = 0; index < 2; index++) {
            const auto& statement = assignmentStatements[index];
            const auto& expectedDot = expected[index];

            CodeManager manager(statement);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            AssignmentExpression assignmentExpression(&manager);
            ASSERT_TRUE(assignmentExpression.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            assignmentExpression.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    { // not feasible to compile
        constexpr array<string_view , 3 > expressions = {"a " , "a =" , "12 := a"} ;
        constexpr array<string_view , 3 > expectedMessages = { "1:3: error: expected \":=\"\n"
                                                             "a \n"
                                                             "  ^~" ,
                                                             "1:3: error: expected :=\n"
                                                             "a =\n"
                                                             "  ^\n" ,
                                                             "1:1: error: expected Identifier Token\n"
                                                             "12 := a\n"
                                                             "^~\n"
        } ;
        for(size_t index = 0 ; index < 3 ; index ++)
        {
            const auto& expression = expressions[index]  ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(expression) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            AssignmentExpression assignmentExpression(&manager) ;
            ASSERT_TRUE(!assignmentExpression.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }
}

TEST(TestParseTree , TestStatement) {
    {
        constexpr array<string_view, 2> statements = {"a := 10", "RETURN a*10"};
        constexpr array<string_view, 2> expected = {"digraph {\n"
                                                    "\tstatement -> assignment-expression;\n"
                                                    "\tassignment-expression -> identifier;\n"
                                                    "\tassignment-expression -> :=;\n"
                                                    "\tassignment-expression -> additive-expression;\n"
                                                    "\tidentifier -> \"a\";\n"
                                                    "\tadditive-expression -> multiplicative-expression;\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> literal;\n"
                                                    "\tliteral -> \"10\";\n"
                                                    "}\n",
                                                    "digraph {\n"
                                                    "\tstatement -> RETURN;\n"
                                                    "\tstatement -> additive-expression;\n"
                                                    "\tadditive-expression -> multiplicative-expression;\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tmultiplicative-expression -> *;\n"
                                                    "\tmultiplicative-expression -> multiplicative-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> identifier;\n"
                                                    "\tidentifier -> \"a\";\n"
                                                    "\tmultiplicative-expression -> unary-expression;\n"
                                                    "\tunary-expression -> primary-expression;\n"
                                                    "\tprimary-expression -> literal;\n"
                                                    "\tliteral -> \"10\";\n"
                                                    "}\n"};
        for (size_t index = 0; index < 2; index++) {
            const auto& statement = statements[index];
            const auto& expectedDot = expected[index];

            CodeManager manager(statement);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            Statement statementNode(&manager);
            ASSERT_TRUE(statementNode.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            statementNode.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    { // not feasible to compile
        constexpr array<string_view , 3> statements = {"VAR 10 + 3" , "ReTuRn 10" , "RETURN a := 10"} ;
        constexpr array<string_view , 3 > expectedMessages =
            {
                "1:1: error: expected RETURN statement or Identifier token\n"
                "VAR 10 + 3\n"
                "^~~\n" ,
                "1:8: error: expected :=\n"
                "ReTuRn 10\n"
                "       ^~\n" ,
                "1:10: error: unexpected token \":=\"\n"
                "RETURN a := 10\n"
                "         ^~\n"
            };
        for(size_t index = 0 ; index < 3 ; index ++) {
            const auto& statement = statements[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(statement) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            Statement parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }


    }
}

TEST(TestParseTree , TestStatementList) {
    {
        constexpr array<string_view, 2> statementLists = {"a := 10", "a:=10;b:=12;\nRETURN a*b"};
        constexpr array<string_view, 2> expected = {
            "digraph {\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> assignment-expression;\n"
            "\tassignment-expression -> identifier;\n"
            "\tassignment-expression -> :=;\n"
            "\tassignment-expression -> additive-expression;\n"
            "\tidentifier -> \"a\";\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"10\";\n"
            "}\n",
            "digraph {\n"
            "\tstatement-list -> statement;\n"
            "\tstatement-list -> ;;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement-list -> ;;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> assignment-expression;\n"
            "\tassignment-expression -> identifier;\n"
            "\tassignment-expression -> :=;\n"
            "\tassignment-expression -> additive-expression;\n"
            "\tidentifier -> \"a\";\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"10\";\n"
            "\tstatement -> assignment-expression;\n"
            "\tassignment-expression -> identifier;\n"
            "\tassignment-expression -> :=;\n"
            "\tassignment-expression -> additive-expression;\n"
            "\tidentifier -> \"b\";\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"12\";\n"
            "\tstatement -> RETURN;\n"
            "\tstatement -> additive-expression;\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tmultiplicative-expression -> *;\n"
            "\tmultiplicative-expression -> multiplicative-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> identifier;\n"
            "\tidentifier -> \"a\";\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> identifier;\n"
            "\tidentifier -> \"b\";\n"
            "}\n"};

        for (size_t index = 0; index < 2; index++) {
            const auto& statementList = statementLists[index];
            const auto& expectedDot = expected[index];
            CodeManager manager(statementList);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            ASSERT_TRUE(!manager.isCodeError());
            StatementList statementListNode(&manager);
            ASSERT_TRUE(statementListNode.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            statementListNode.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    { // Not feasible to compile 

        constexpr array<string_view , 3> statementLists = {"" , "a := 1 , b := 2" , "a := 1 ; RETURN a ;"} ;
        constexpr array<string_view , 3> expectedMessages = {
            "1:1: error: expected \"Return or Identifier token\"\n"
            "\n"
            "^" ,
            "1:8: error: unexpected token \",\"\n"
            "a := 1 , b := 2\n"
            "       ^\n" ,
            "1:20: error: expected \"Return or Identifier token\"\n"
            "a := 1 ; RETURN a ;\n"
            "                   ^"
        };
        for(size_t index = 0 ; index < 3 ; index ++) {
            const auto& statementList = statementLists[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(statementList) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            StatementList parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }

    }
}

TEST(TestParseTree , TestCompoundStatement){
    {
        constexpr array<string_view, 2> compoundStatements = {"BEGIN a := 10 END", "BEGIN RETURN a*10 END"};
        constexpr array<string_view, 2> expected = {
            "digraph {\n"
            "\tcompound-statement -> BEGIN;\n"
            "\tcompound-statement -> statement-list;\n"
            "\tcompound-statement -> END;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> assignment-expression;\n"
            "\tassignment-expression -> identifier;\n"
            "\tassignment-expression -> :=;\n"
            "\tassignment-expression -> additive-expression;\n"
            "\tidentifier -> \"a\";\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"10\";\n"
            "}\n",
            "digraph {\n"
            "\tcompound-statement -> BEGIN;\n"
            "\tcompound-statement -> statement-list;\n"
            "\tcompound-statement -> END;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> RETURN;\n"
            "\tstatement -> additive-expression;\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tmultiplicative-expression -> *;\n"
            "\tmultiplicative-expression -> multiplicative-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> identifier;\n"
            "\tidentifier -> \"a\";\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"10\";\n"
            "}\n"};
        for (size_t index = 0; index < 2; index++) {
            const auto& compoundStatement = compoundStatements[index];
            const auto& expectedDot = expected[index];
            CodeManager manager(compoundStatement);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            CompoundStatement compoundStatementNode(&manager);
            ASSERT_TRUE(compoundStatementNode.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            compoundStatementNode.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    { // Not feasible to compile

        constexpr array<string_view , 6> compoundStatements = {"" , "BEGIN END" ,
                                                               "PARAM " , " RETURN 10 END" ,
                                                               "BEGIN RETURN 10" , "begiN RETURN 10 end"} ;
        constexpr array<string_view , 6> expectedMessages = {
            "1:1: error: expected \"BEGIN\"\n"
            "\n"
            "^~~~~" ,
            "1:7: error: expected RETURN statement or Identifier token\n"
            "BEGIN END\n"
            "      ^~~\n" ,
            "1:1: error: expected BEGIN\n"
            "PARAM \n"
            "^~~~~\n" ,
            "1:1: error: expected BEGIN\n"
            "RETURN 10 END\n"
            "^~~~~~\n" ,
            "1:16: error: expected \"END\"\n"
            "BEGIN RETURN 10\n"
            "               ^~~" ,
            "1:1: error: expected BEGIN\n"
            "begiN RETURN 10 end\n"
            "^~~~~\n"
        };
        for(size_t index = 0 ; index < 3 ; index ++) {
            const auto& compoundStatement = compoundStatements[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(compoundStatement) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            CompoundStatement parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }

    }
}

TEST(TestParseTree , TestInitDeclarator) {
    {
        constexpr string_view declarator = "a = 10\n";
        constexpr string_view expected = "digraph {\n"
                                         "\tinit-declarator -> identifier;\n"
                                         "\tinit-declarator -> =;\n"
                                         "\tinit-declarator -> literal;\n"
                                         "\tidentifier -> \"a\";\n"
                                         "\tliteral -> \"10\";\n"
                                         "}\n";
        CodeManager manager(declarator);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        InitDeclarator initDeclarator(&manager);
        ASSERT_TRUE(initDeclarator.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        initDeclarator.accept(printVisitor);
        ASSERT_EQ(expected, printVisitor.getOutput());
    }
    { // Not feasible to compile

        constexpr array<string_view , 8> initDeclaratorCases = {"" , "VAR = 10" , "x := 9" , "x = y" , "= 10" , "x 10" , "x = " , "x"} ;
        constexpr array<string_view , 8> expectedMessages = {
            "1:1: error: expected \"Identifier Token\"\n"
            "\n"
            "^" ,
            "1:1: error: expected Identifier Token\n"
            "VAR = 10\n"
            "^~~\n" ,
            "1:3: error: expected =\n"
            "x := 9\n"
            "  ^~\n" ,
            "1:5: error: expected Literal Token\n"
            "x = y\n"
            "    ^\n" ,
            "1:1: error: expected Identifier Token\n"
            "= 10\n"
            "^\n" ,
            "1:3: error: expected =\n"
            "x 10\n"
            "  ^~\n" ,
            "1:5: error: expected \"Literal Token\"\n"
            "x = \n"
            "    ^" ,
            "1:2: error: expected \"=\"\n"
            "x\n"
            " ^"
        };
        for(size_t index = 0 ; index < 8 ; index ++) {
            const auto& initDeclarator = initDeclaratorCases[index] ;
            const auto& expected = expectedMessages[index] ;
            {
                CodeManager manager(initDeclarator);
                TokenStream tokenStream(&manager);
                tokenStream.compileCode();
                InitDeclarator parseTreeNode(&manager);
                ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream));
                ASSERT_TRUE(manager.isCodeError());
                ASSERT_EQ(expected, manager.error_message());
            }
            {
                CodeManager manager(initDeclarator) ;
                TokenStream tokenStream(&manager) ;
                tokenStream.compileCode() ;
                InitDeclaratorList parseTreeNode(&manager) ;
                ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
                ASSERT_TRUE(manager.isCodeError()) ;
                ASSERT_EQ(expected , manager.error_message()) ;

            }
        }
    }

}

TEST(TestParseTree , TestInitDeclaratorList) {
    {
        constexpr array<string_view, 2> declaratorList = {"a=10\n", "a=10 ,b =9"};
        constexpr array<string_view, 2> expected = {"digraph {\n"
                                                    "\tinit-declarator-list -> init-declarator;\n"
                                                    "\tinit-declarator -> identifier;\n"
                                                    "\tinit-declarator -> =;\n"
                                                    "\tinit-declarator -> literal;\n"
                                                    "\tidentifier -> \"a\";\n"
                                                    "\tliteral -> \"10\";\n"
                                                    "}\n",
                                                    "digraph {\n"
                                                    "\tinit-declarator-list -> init-declarator;\n"
                                                    "\tinit-declarator-list -> ,;\n"
                                                    "\tinit-declarator-list -> init-declarator;\n"
                                                    "\tinit-declarator -> identifier;\n"
                                                    "\tinit-declarator -> =;\n"
                                                    "\tinit-declarator -> literal;\n"
                                                    "\tidentifier -> \"a\";\n"
                                                    "\tliteral -> \"10\";\n"
                                                    "\tinit-declarator -> identifier;\n"
                                                    "\tinit-declarator -> =;\n"
                                                    "\tinit-declarator -> literal;\n"
                                                    "\tidentifier -> \"b\";\n"
                                                    "\tliteral -> \"9\";\n"
                                                    "}\n"};
        for (size_t index = 0; index < 2; index++) {
            const auto& declarator = declaratorList[index];
            const auto& expectedDot = expected[index];
            CodeManager manager(declarator);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            InitDeclaratorList initDeclaratorList(&manager);
            ASSERT_TRUE(initDeclaratorList.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            initDeclaratorList.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    { // Not feasible to compile

        constexpr array<string_view , 2> initDeclaratorCases = {"a = 10 ; b = 5" , "a = 10 , b = 5 , "} ;
        constexpr array<string_view , 2> expectedMessages = {
            "1:8: error: unexpected token \";\"\n"
            "a = 10 ; b = 5\n"
            "       ^\n" ,
            "1:18: error: expected \"Identifier Token\"\n"
            "a = 10 , b = 5 , \n"
            "                 ^"
        };
        for(size_t index = 0 ; index < 2 ; index ++) {
            const auto& initDeclarator = initDeclaratorCases[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(initDeclarator) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            InitDeclaratorList parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }

}

TEST(TestParseTree , TestDeclaratorList) {
    {
        constexpr array<string_view, 2> decList = {"volume", "index , value"};
        constexpr array<string_view, 2> expected = {
            "digraph {\n"
            "\tdeclarator-list -> identifier;\n"
            "\tidentifier -> \"volume\";\n"
            "}\n",
            "digraph {\n"
            "\tdeclarator-list -> identifier;\n"
            "\tdeclarator-list -> ,;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tidentifier -> \"index\";\n"
            "\tidentifier -> \"value\";\n"
            "}\n"};
        for (size_t index = 0; index < 2; index++) {
            const auto& declarator = decList[index];
            const auto& expectedDot = expected[index];
            CodeManager manager(declarator);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            DeclaratorList declaratorList(&manager);
            ASSERT_TRUE(declaratorList.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            declaratorList.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    { // Not feasible to compile

        constexpr array<string_view , 3> declaratorCases = {"" , "a ; b" , "a , b , "} ;
        constexpr array<string_view , 3> expectedMessages = {
            "1:1: error: expected \"Identifier Token\"\n"
            "\n"
            "^" ,
            "1:3: error: unexpected token \";\"\n"
            "a ; b\n"
            "  ^\n" ,
            "1:9: error: expected \"Identifier Token\"\n"
            "a , b , \n"
            "        ^"
        };
        for(size_t index = 0 ; index < 3 ; index ++) {
            const auto& initDeclarator = declaratorCases[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(initDeclarator) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            DeclaratorList parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }

}

TEST(TestParseTree , TestConstantDeclaration) {
    {
        constexpr string_view constDeclaration = "CONST a=10, b=12;\n";
        constexpr string_view expected = "digraph {\n"
                                         "\tconstant-declaration -> CONST;\n"
                                         "\tconstant-declaration -> init-declarator-list;\n"
                                         "\tconstant-declaration -> ;;\n"
                                         "\tinit-declarator-list -> init-declarator;\n"
                                         "\tinit-declarator-list -> ,;\n"
                                         "\tinit-declarator-list -> init-declarator;\n"
                                         "\tinit-declarator -> identifier;\n"
                                         "\tinit-declarator -> =;\n"
                                         "\tinit-declarator -> literal;\n"
                                         "\tidentifier -> \"a\";\n"
                                         "\tliteral -> \"10\";\n"
                                         "\tinit-declarator -> identifier;\n"
                                         "\tinit-declarator -> =;\n"
                                         "\tinit-declarator -> literal;\n"
                                         "\tidentifier -> \"b\";\n"
                                         "\tliteral -> \"12\";\n"
                                         "}\n";
        CodeManager manager(constDeclaration);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        ConstantDeclaration constantDeclaration(&manager);
        ASSERT_TRUE(constantDeclaration.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        constantDeclaration.accept(printVisitor);
        ASSERT_EQ(expected, printVisitor.getOutput());
    }
    { // failed but without syntax error
        constexpr array<string_view , 2 > constDeclarations = {"" , "VAR a , b ;"} ;
        for(const auto& constDecl : constDeclarations) {
            CodeManager manager(constDecl);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            ConstantDeclaration constantDeclaration(&manager);
            ASSERT_TRUE(!constantDeclaration.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
        }
    }
    { // Not feasible to compile

        constexpr array<string_view , 5> constantDeclarations = {
            "CONST a,b;" ,
            "CONST a := 9 ;" ,
            "CONST a = 5 " ,
            "CONST a = b ;" ,
            "CONST 12 = b ;"
        } ;
        constexpr array<string_view , 5> expectedMessages = {
            "1:8: error: expected =\n"
            "CONST a,b;\n"
            "       ^\n",
            "1:9: error: expected =\n"
            "CONST a := 9 ;\n"
            "        ^~\n" ,
            "1:13: error: expected \";\"\n"
            "CONST a = 5 \n"
            "            ^" ,
            "1:11: error: expected Literal Token\n"
            "CONST a = b ;\n"
            "          ^\n" ,
            "1:7: error: expected Identifier Token\n"
            "CONST 12 = b ;\n"
            "      ^~\n"

        };
        for(size_t index = 0 ; index < 5 ; index ++) {
            const auto& constantDeclaration = constantDeclarations[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(constantDeclaration) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            ConstantDeclaration parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }
}

TEST(TestParseTree , TestVariableDeclaration) {
    {
        constexpr string_view varDeclaration = "VAR a, b;\n";
        constexpr string_view expected = "digraph {\n"
                                         "\tvariable-declaration -> VAR;\n"
                                         "\tvariable-declaration -> declarator-list;\n"
                                         "\tvariable-declaration -> ;;\n"
                                         "\tdeclarator-list -> identifier;\n"
                                         "\tdeclarator-list -> ,;\n"
                                         "\tdeclarator-list -> identifier;\n"
                                         "\tidentifier -> \"a\";\n"
                                         "\tidentifier -> \"b\";\n"
                                         "}\n";
        CodeManager manager(varDeclaration);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        VariableDeclaration variableDeclaration(&manager);
        ASSERT_TRUE(variableDeclaration.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        variableDeclaration.accept(printVisitor);
        ASSERT_EQ(expected, printVisitor.getOutput());
    }
    { // failed but without syntax error
        constexpr array<string_view , 2 > variableDeclarations = {"" , "CONST a , b ;"} ;
        for(const auto& currentCase : variableDeclarations) {
            CodeManager manager(currentCase);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            VariableDeclaration variableDeclaration(&manager);
            ASSERT_TRUE(!variableDeclaration.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
        }
    }
    { // Not feasible to compile

        constexpr array<string_view , 4> variableDeclarations = {
            "VAR a = 10,b = 5;" ,
            "VAR a := 9 ;" ,
            "VAR a " ,
            "VAR 12 ;"
        } ;
        constexpr array<string_view , 4> expectedMessages = {
            "1:7: error: expected ;\n"
            "VAR a = 10,b = 5;\n"
            "      ^\n" ,
            "1:7: error: expected ;\n"
            "VAR a := 9 ;\n"
            "      ^~\n" ,
            "1:7: error: expected \";\"\n"
            "VAR a \n"
            "      ^" ,
            "1:5: error: expected Identifier Token\n"
            "VAR 12 ;\n"
            "    ^~\n"
        };
        for(size_t index = 0 ; index < 4 ; index ++) {
            const auto& variableDeclaration = variableDeclarations[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(variableDeclaration) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            VariableDeclaration parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }

}

TEST(TestParseTree , TestParameterDeclaration) {
    {
        constexpr string_view paramDeclaration = "PARAM a, b;\n";
        string expected = "digraph {\n"
                          "\tparameter-declaration -> PARAM;\n"
                          "\tparameter-declaration -> declarator-list;\n"
                          "\tparameter-declaration -> ;;\n"
                          "\tdeclarator-list -> identifier;\n"
                          "\tdeclarator-list -> ,;\n"
                          "\tdeclarator-list -> identifier;\n"
                          "\tidentifier -> \"a\";\n"
                          "\tidentifier -> \"b\";\n"
                          "}\n";
        CodeManager manager(paramDeclaration);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        ParameterDeclaration parameterDeclaration(&manager);
        ASSERT_TRUE(parameterDeclaration.compileCode(lexicalAnalyzer));
        ASSERT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        parameterDeclaration.accept(printVisitor);
        ASSERT_EQ(expected, printVisitor.getOutput());
    }
    { // failed but without syntax error
        constexpr array<string_view , 2 > parameterDeclarations = {"" , "VAR a , b ;"} ;
        for(const auto& currentCase : parameterDeclarations) {
            CodeManager manager(currentCase);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            ParameterDeclaration parameterDeclaration(&manager);
            ASSERT_TRUE(!parameterDeclaration.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
        }
    }
    { // Not feasible to compile

        constexpr array<string_view , 4> parameterDeclarations = {
            "PARAM a = 10,b = 5;" ,
            "PARAM a := 9 ;" ,
            "PARAM a " ,
            "PARAM 12 ;"
        } ;
        constexpr array<string_view , 4> expectedMessages = {
            "1:9: error: expected ;\n"
            "PARAM a = 10,b = 5;\n"
            "        ^\n" ,
            "1:9: error: expected ;\n"
            "PARAM a := 9 ;\n"
            "        ^~\n" ,
            "1:9: error: expected \";\"\n"
            "PARAM a \n"
            "        ^" ,
            "1:7: error: expected Identifier Token\n"
            "PARAM 12 ;\n"
            "      ^~\n"
        };
        for(size_t index = 0 ; index < 4 ; index ++) {
            const auto& parameterDeclaration = parameterDeclarations[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(parameterDeclaration) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            ParameterDeclaration parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }

}

TEST(TestParseTree , TestFunctionDeclaration) {
    {
        constexpr array<string_view, 4> functions =
            {
                "PARAM a,b;\n"
                "VAR c,d ; \n"
                "CONST e=1,f=2;\n"
                "BEGIN\n"
                "RETURN 1\n"
                "END\n"
                ".",
                "VAR c,d ; \n"
                "CONST e=1,f=2;\n"
                "BEGIN\n"
                "RETURN 1\n"
                "END\n"
                ".",
                "CONST e=1,f=2;\n"
                "BEGIN\n"
                "RETURN 1\n"
                "END\n"
                ".",
                "BEGIN\n"
                "RETURN 1\n"
                "END\n"
                "."};
        constexpr array<string_view, 4> expected = {
            "digraph {\n"
            "\tfunction-declaration -> parameter-declaration;\n"
            "\tfunction-declaration -> variable-declaration;\n"
            "\tfunction-declaration -> constant-declaration;\n"
            "\tfunction-declaration -> compound-declaration;\n"
            "\tfunction-declaration -> .;\n"
            "\tparameter-declaration -> PARAM;\n"
            "\tparameter-declaration -> declarator-list;\n"
            "\tparameter-declaration -> ;;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tdeclarator-list -> ,;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tidentifier -> \"a\";\n"
            "\tidentifier -> \"b\";\n"
            "\tvariable-declaration -> VAR;\n"
            "\tvariable-declaration -> declarator-list;\n"
            "\tvariable-declaration -> ;;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tdeclarator-list -> ,;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tidentifier -> \"c\";\n"
            "\tidentifier -> \"d\";\n"
            "\tconstant-declaration -> CONST;\n"
            "\tconstant-declaration -> init-declarator-list;\n"
            "\tconstant-declaration -> ;;\n"
            "\tinit-declarator-list -> init-declarator;\n"
            "\tinit-declarator-list -> ,;\n"
            "\tinit-declarator-list -> init-declarator;\n"
            "\tinit-declarator -> identifier;\n"
            "\tinit-declarator -> =;\n"
            "\tinit-declarator -> literal;\n"
            "\tidentifier -> \"e\";\n"
            "\tliteral -> \"1\";\n"
            "\tinit-declarator -> identifier;\n"
            "\tinit-declarator -> =;\n"
            "\tinit-declarator -> literal;\n"
            "\tidentifier -> \"f\";\n"
            "\tliteral -> \"2\";\n"
            "\tcompound-statement -> BEGIN;\n"
            "\tcompound-statement -> statement-list;\n"
            "\tcompound-statement -> END;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> RETURN;\n"
            "\tstatement -> additive-expression;\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"1\";\n"
            "}\n",
            "digraph {\n"
            "\tfunction-declaration -> variable-declaration;\n"
            "\tfunction-declaration -> constant-declaration;\n"
            "\tfunction-declaration -> compound-declaration;\n"
            "\tfunction-declaration -> .;\n"
            "\tvariable-declaration -> VAR;\n"
            "\tvariable-declaration -> declarator-list;\n"
            "\tvariable-declaration -> ;;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tdeclarator-list -> ,;\n"
            "\tdeclarator-list -> identifier;\n"
            "\tidentifier -> \"c\";\n"
            "\tidentifier -> \"d\";\n"
            "\tconstant-declaration -> CONST;\n"
            "\tconstant-declaration -> init-declarator-list;\n"
            "\tconstant-declaration -> ;;\n"
            "\tinit-declarator-list -> init-declarator;\n"
            "\tinit-declarator-list -> ,;\n"
            "\tinit-declarator-list -> init-declarator;\n"
            "\tinit-declarator -> identifier;\n"
            "\tinit-declarator -> =;\n"
            "\tinit-declarator -> literal;\n"
            "\tidentifier -> \"e\";\n"
            "\tliteral -> \"1\";\n"
            "\tinit-declarator -> identifier;\n"
            "\tinit-declarator -> =;\n"
            "\tinit-declarator -> literal;\n"
            "\tidentifier -> \"f\";\n"
            "\tliteral -> \"2\";\n"
            "\tcompound-statement -> BEGIN;\n"
            "\tcompound-statement -> statement-list;\n"
            "\tcompound-statement -> END;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> RETURN;\n"
            "\tstatement -> additive-expression;\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"1\";\n"
            "}\n",
            "digraph {\n"
            "\tfunction-declaration -> constant-declaration;\n"
            "\tfunction-declaration -> compound-declaration;\n"
            "\tfunction-declaration -> .;\n"
            "\tconstant-declaration -> CONST;\n"
            "\tconstant-declaration -> init-declarator-list;\n"
            "\tconstant-declaration -> ;;\n"
            "\tinit-declarator-list -> init-declarator;\n"
            "\tinit-declarator-list -> ,;\n"
            "\tinit-declarator-list -> init-declarator;\n"
            "\tinit-declarator -> identifier;\n"
            "\tinit-declarator -> =;\n"
            "\tinit-declarator -> literal;\n"
            "\tidentifier -> \"e\";\n"
            "\tliteral -> \"1\";\n"
            "\tinit-declarator -> identifier;\n"
            "\tinit-declarator -> =;\n"
            "\tinit-declarator -> literal;\n"
            "\tidentifier -> \"f\";\n"
            "\tliteral -> \"2\";\n"
            "\tcompound-statement -> BEGIN;\n"
            "\tcompound-statement -> statement-list;\n"
            "\tcompound-statement -> END;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> RETURN;\n"
            "\tstatement -> additive-expression;\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"1\";\n"
            "}\n",
            "digraph {\n"
            "\tfunction-declaration -> compound-declaration;\n"
            "\tfunction-declaration -> .;\n"
            "\tcompound-statement -> BEGIN;\n"
            "\tcompound-statement -> statement-list;\n"
            "\tcompound-statement -> END;\n"
            "\tstatement-list -> statement;\n"
            "\tstatement -> RETURN;\n"
            "\tstatement -> additive-expression;\n"
            "\tadditive-expression -> multiplicative-expression;\n"
            "\tmultiplicative-expression -> unary-expression;\n"
            "\tunary-expression -> primary-expression;\n"
            "\tprimary-expression -> literal;\n"
            "\tliteral -> \"1\";\n"
            "}\n"};
        for (size_t index = 0; index < 4; index++) {
            const auto& currFunction = functions[index];
            const auto& expectedDot = expected[index];

            CodeManager manager(currFunction);
            TokenStream lexicalAnalyzer(&manager);
            lexicalAnalyzer.compileCode();
            FunctionDeclaration functionDeclaration(&manager);
            ASSERT_TRUE(functionDeclaration.compileCode(lexicalAnalyzer));
            ASSERT_TRUE(!manager.isCodeError());
            TestPrintVisitor printVisitor;
            functionDeclaration.accept(printVisitor);
            ASSERT_EQ(expectedDot, printVisitor.getOutput());
        }
    }
    {
        constexpr array<string_view, 5> functions =
            {
                "",
                "BEGIN END." ,
                "BEGIN RETURN END." ,
                "BEGIN RETURN 1." ,
                "BEGIN RETURN 1 END"
            };
        constexpr array<string_view , 5 > expectedMessages = {
            "1:1: error: expected \"BEGIN\"\n"
            "\n"
            "^~~~~" ,
            "1:7: error: expected RETURN statement or Identifier token\n"
            "BEGIN END.\n"
            "      ^~~\n" ,
            "1:14: error: expected Identifier , Literal or Open Bracket\n"
            "BEGIN RETURN END.\n"
            "             ^~~\n" ,
            "1:15: error: expected END\n"
            "BEGIN RETURN 1.\n"
            "              ^\n" ,
            "1:19: error: expected \".\"\n"
            "BEGIN RETURN 1 END\n"
            "                  ^"
        };
        for(size_t index = 0 ; index < 5 ; ++index) {
            const auto& curFunction = functions[index] ;
            const auto& expected = expectedMessages[index] ;
            CodeManager manager(curFunction) ;
            TokenStream tokenStream(&manager) ;
            tokenStream.compileCode() ;
            FunctionDeclaration parseTreeNode(&manager) ;
            ASSERT_TRUE(!parseTreeNode.compileCode(tokenStream)) ;
            ASSERT_TRUE(manager.isCodeError()) ;
            ASSERT_EQ(expected , manager.error_message()) ;
        }
    }
}
