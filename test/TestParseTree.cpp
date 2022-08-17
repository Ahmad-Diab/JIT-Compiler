#include "pljit/PrintParseTreeVisitor.hpp"
#include "pljit/ParseTree.hpp"
#include <gtest/gtest.h>

using namespace std ;
using namespace jitcompiler ;

TEST(TestParseTree , TestPrimaryExpression) {
    constexpr array<string_view , 4> terminalExpression = {"volume", "1213", "(volume)", "(1213)"};
    constexpr array<string_view , 4> expectedPrimary = {"digraph {\n"
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
    constexpr array<string_view , 4> expectedUnary = {"digraph {\n"
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
    constexpr array<string_view , 4> expectedMult = {"digraph {\n"
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
    constexpr array<string_view , 4> expectedAdd = {"digraph {\n"
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
        EXPECT_TRUE(primaryExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        primaryExpression.accept(printVisitor);
        EXPECT_EQ(expected, printVisitor.getOutput());
    }
    for (size_t indexUnary = 0; indexUnary < 4; indexUnary++) {
        const auto& cur = terminalExpression[indexUnary];
        const auto& expected = expectedUnary[indexUnary];
        CodeManager manager(cur);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        UnaryExpression unaryExpression(&manager);
        EXPECT_TRUE(unaryExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        unaryExpression.accept(printVisitor);
        EXPECT_EQ(expected, printVisitor.getOutput());
    }
    for (size_t indexMult = 0; indexMult < 4; indexMult++) {
        const auto& cur = terminalExpression[indexMult];
        const auto& expected = expectedMult[indexMult];
        CodeManager manager(cur);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        MultiplicativeExpression primaryExpression(&manager);
        EXPECT_TRUE(primaryExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        primaryExpression.accept(printVisitor);
        EXPECT_EQ(expected, printVisitor.getOutput());
    }
    for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
        const auto& cur = terminalExpression[indexAdd];
        const auto& expected = expectedAdd[indexAdd];
        CodeManager manager(cur);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AdditiveExpression primaryExpression(&manager);
        EXPECT_TRUE(primaryExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        primaryExpression.accept(printVisitor);
        EXPECT_EQ(expected, printVisitor.getOutput());
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
        EXPECT_TRUE(unaryExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        unaryExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
    for (size_t indexMult = 0; indexMult < 4; indexMult++) {
        const auto& expression = unaryExpressions[indexMult];
        const auto& expectedDot = expectedMult[indexMult];
        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        MultiplicativeExpression multiplicativeExpression(&manager);
        EXPECT_TRUE(multiplicativeExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        multiplicativeExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
    for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
        const auto& expression = unaryExpressions[indexAdd];
        const auto& expectedDot = expectedAdd[indexAdd];
        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AdditiveExpression additiveExpression(&manager);
        EXPECT_TRUE(additiveExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        additiveExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
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
        EXPECT_TRUE(multiplicativeExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        multiplicativeExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
    for (size_t indexAdd = 0; indexAdd < 4; indexAdd++) {
        const auto& expression = multExpressions[indexAdd];
        const auto& expectedDot = expectedAdd[indexAdd];

        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AdditiveExpression additiveExpression(&manager);
        EXPECT_TRUE(additiveExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        additiveExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
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
        EXPECT_TRUE(additiveExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        additiveExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
    { // Test primary-expression := "(" additive-expression ")"
        const auto& expression = expressions[1];
        const auto& expectedDot = expectedPrimary;

        CodeManager manager(expression);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        PrimaryExpression primaryExpression(&manager);
        EXPECT_TRUE(primaryExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        primaryExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestAssignmentExpression) {
    constexpr array<string_view , 2> assignmentStatements = { "volume := 1234" , "volume :=  w * h * d"} ;
    constexpr array<string_view , 2> expected = {"digraph {\n"
                                      "\tassignment-expression -> identifier;\n"
                                      "\tassignment-expression -> :=;\n"
                                      "\tassignment-expression -> additive-expression;\n"
                                      "\tidentifier -> \"volume\";\n"
                                      "\tadditive-expression -> multiplicative-expression;\n"
                                      "\tmultiplicative-expression -> unary-expression;\n"
                                      "\tunary-expression -> primary-expression;\n"
                                      "\tprimary-expression -> literal;\n"
                                      "\tliteral -> \"1234\";\n"
                                      "}\n" ,
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
                                      "}\n" };
    for (size_t index = 0; index < 2; index++) {
        const auto& statement = assignmentStatements[index];
        const auto& expectedDot = expected[index];

        CodeManager manager(statement);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        AssignmentExpression assignmentExpression(&manager);
        EXPECT_TRUE(assignmentExpression.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        assignmentExpression.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestStatement)
{
    constexpr array<string_view , 2> statements = {"a := 10" , "RETURN a*10"} ;
    constexpr array<string_view , 2> expected = {"digraph {\n"
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
                                      "}\n"
                                      ,
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
        EXPECT_TRUE(statementNode.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        statementNode.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestStatementList) {
    constexpr array<string_view , 2> statementLists = {"a := 10" , "a:=10;b:=12;\nRETURN a*b"} ;
    constexpr array<string_view , 2> expected = {
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
        "}\n" ,
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
        "}\n"
    };

    for (size_t index = 0; index < 2; index++) {
        const auto& statementList = statementLists[index];
        const auto& expectedDot = expected[index];
        CodeManager manager(statementList);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        StatementList statementListNode(&manager);
        EXPECT_TRUE(statementListNode.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        statementListNode.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestCompoundStatement){
    constexpr array<string_view , 2> compoundStatements = {"BEGIN a := 10 END" , "BEGIN RETURN a*10 END"} ;
    constexpr array<string_view , 2> expected = {
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
        "}\n" ,
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
        "}\n"
    };
    for (size_t index = 0; index < 2; index++) {
        const auto& compoundStatement = compoundStatements[index];
        const auto& expectedDot = expected[index];
        CodeManager manager(compoundStatement);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        CompoundStatement compoundStatementNode(&manager);
        EXPECT_TRUE(compoundStatementNode.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        compoundStatementNode.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestInitDeclarator) {
    constexpr string_view declarator = "a = 10\n" ;
    constexpr string_view expected = "digraph {\n"
                                     "\tinit-declarator -> identifier;\n"
                                     "\tinit-declarator -> =;\n"
                                     "\tinit-declarator -> literal;\n"
                                     "\tidentifier -> \"a\";\n"
                                     "\tliteral -> \"10\";\n"
                                     "}\n" ;
    CodeManager manager(declarator);
    TokenStream lexicalAnalyzer(&manager);
    lexicalAnalyzer.compileCode();
    InitDeclartor initDeclarator(&manager);
    EXPECT_TRUE(initDeclarator.recursiveDecentParser(lexicalAnalyzer));
    EXPECT_TRUE(!manager.isCodeError());
    TestPrintVisitor printVisitor;
    initDeclarator.accept(printVisitor);
    EXPECT_EQ(expected, printVisitor.getOutput());
}

TEST(TestParseTree , TestInitDeclaratorList) {
    constexpr array<string_view , 2> declaratorList = {"a=10\n" , "a=10 ,b =9"} ;
    constexpr array<string_view , 2> expected = {"digraph {\n"
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
                                                "}\n"
    };
    for(size_t index = 0 ; index < 2 ; index++) {
        const auto& declarator = declaratorList[index] ;
        const auto& expectedDot = expected[index] ;
        CodeManager manager(declarator);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        InitDeclartorList initDeclaratorList(&manager);
        EXPECT_TRUE(initDeclaratorList.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        initDeclaratorList.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }

}

TEST(TestParseTree , TestDeclaratorList) {
    constexpr array<string_view , 2> decList = {"volume" , "index , value"} ;
    constexpr array<string_view , 2> expected = {
        "digraph {\n"
        "\tdeclarator-list -> identifier;\n"
        "\tidentifier -> \"volume\";\n"
        "}\n" ,
        "digraph {\n"
        "\tdeclarator-list -> identifier;\n"
        "\tdeclarator-list -> ,;\n"
        "\tdeclarator-list -> identifier;\n"
        "\tidentifier -> \"index\";\n"
        "\tidentifier -> \"value\";\n"
        "}\n"
    };
    for(size_t index = 0 ; index < 2 ; index++) {
        const auto& declarator = decList[index] ;
        const auto& expectedDot = expected[index] ;
        CodeManager manager(declarator);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        DeclartorList declartorList(&manager);
        EXPECT_TRUE(declartorList.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        declartorList.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());
    }
}

TEST(TestParseTree , TestConstantDeclaration){
    constexpr string_view constDeclaration  = "CONST a=10, b=12;\n" ;
    constexpr string_view expected =  "digraph {\n"
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
                                     "}\n" ;
    CodeManager manager(constDeclaration);
    TokenStream lexicalAnalyzer(&manager);
    lexicalAnalyzer.compileCode();
    ConstantDeclaration constantDeclaration(&manager);
    EXPECT_TRUE(constantDeclaration.recursiveDecentParser(lexicalAnalyzer));
    EXPECT_TRUE(!manager.isCodeError());
    TestPrintVisitor printVisitor;
    constantDeclaration.accept(printVisitor);
    EXPECT_EQ(expected, printVisitor.getOutput());
}

TEST(TestParseTree , TestVariableDeclaration) {
    constexpr string_view varDeclaration  = "VAR a, b;\n" ;
    constexpr string_view expected =  "digraph {\n"
                                     "\tvariable-declaration -> VAR;\n"
                                     "\tvariable-declaration -> declarator-list;\n"
                                     "\tvariable-declaration -> ;;\n"
                                     "\tdeclarator-list -> identifier;\n"
                                     "\tdeclarator-list -> ,;\n"
                                     "\tdeclarator-list -> identifier;\n"
                                     "\tidentifier -> \"a\";\n"
                                     "\tidentifier -> \"b\";\n"
                                     "}\n" ;
    CodeManager manager(varDeclaration);
    TokenStream lexicalAnalyzer(&manager);
    lexicalAnalyzer.compileCode();
    VariableDeclaration variableDeclaration(&manager);
    EXPECT_TRUE(variableDeclaration.recursiveDecentParser(lexicalAnalyzer));
    EXPECT_TRUE(!manager.isCodeError());
    TestPrintVisitor printVisitor;
    variableDeclaration.accept(printVisitor);
    EXPECT_EQ(expected, printVisitor.getOutput());
}

TEST(TestParseTree , TestParameterDeclaration) {
    constexpr string_view paramDeclaration  = "PARAM a, b;\n" ;
    string expected =  "digraph {\n"
                                     "\tparameter-declaration -> PARAM;\n"
                                     "\tparameter-declaration -> declarator-list;\n"
                                     "\tparameter-declaration -> ;;\n"
                                     "\tdeclarator-list -> identifier;\n"
                                     "\tdeclarator-list -> ,;\n"
                                     "\tdeclarator-list -> identifier;\n"
                                     "\tidentifier -> \"a\";\n"
                                     "\tidentifier -> \"b\";\n"
                                     "}\n" ;
    CodeManager manager(paramDeclaration);
    TokenStream lexicalAnalyzer(&manager);
    lexicalAnalyzer.compileCode();
    ParameterDeclaration parameterDeclaration(&manager);
    EXPECT_TRUE(parameterDeclaration.recursiveDecentParser(lexicalAnalyzer));
    EXPECT_TRUE(!manager.isCodeError());
    TestPrintVisitor printVisitor;
    parameterDeclaration.accept(printVisitor);
    EXPECT_EQ(expected, printVisitor.getOutput());
}

TEST(TestParseTree , TestFunctionDeclaration) {
    constexpr array<string_view , 4> functions =
        {
            "PARAM a,b;\n"
            "VAR c,d ; \n"
            "CONST e=1,f=2;\n"
            "BEGIN\n"
            "RETURN 1\n"
            "END\n"
            "."
            ,
            "VAR c,d ; \n"
            "CONST e=1,f=2;\n"
            "BEGIN\n"
            "RETURN 1\n"
            "END\n"
            "."
            ,
            "CONST e=1,f=2;\n"
            "BEGIN\n"
            "RETURN 1\n"
            "END\n"
            "."
            ,
            "BEGIN\n"
            "RETURN 1\n"
            "END\n"
            "."
        };
    constexpr array<string_view , 4> expected = {
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
        "}\n"
        ,
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
        "}\n"
        ,
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
        "}\n" ,
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
        "}\n"
    };
    for(size_t index = 0 ; index < 4 ; index ++) {
        const auto& currFunction = functions[index] ;
        const auto& expectedDot = expected[index] ;

        CodeManager manager(currFunction);
        TokenStream lexicalAnalyzer(&manager);
        lexicalAnalyzer.compileCode();
        FunctionDeclaration functionDeclaration(&manager);
        EXPECT_TRUE(functionDeclaration.recursiveDecentParser(lexicalAnalyzer));
        EXPECT_TRUE(!manager.isCodeError());
        TestPrintVisitor printVisitor;
        functionDeclaration.accept(printVisitor);
        EXPECT_EQ(expectedDot, printVisitor.getOutput());

    }

}
