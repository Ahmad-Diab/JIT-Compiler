#include <gtest/gtest.h>

#include "pljit/management/CodeManager.hpp"
#include "pljit/semantic/AST.hpp"
#include "pljit/semantic/EvaluationContext.hpp"
#include "pljit/syntax/ParseTree.hpp"
#include "pljit/syntax/TokenStream.hpp"

using namespace std ;
using namespace jitcompiler ;
using namespace jitcompiler ::management;
using namespace jitcompiler ::syntax;
using namespace jitcompiler ::semantic;

TEST(TestEvaluation , TestLiteral) {
    constexpr string_view code =
        "BEGIN\n"
        "RETURN 1\n"
        "END.\n" ;
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
    SymbolTable& symbolTable = functionAst.getSymbolTable() ;
    EvaluationContext evaluationContext(symbolTable) ;
    optional<int64_t> val = functionAst.evaluate(evaluationContext) ;
    ASSERT_TRUE(val.value()) ;
    ASSERT_EQ(val.value() , 1) ;
}
TEST(TestEvaluation , Identifier) {
    constexpr string_view code =
        "PARAM a;\n"
        "BEGIN\n"
        "RETURN a + 1\n"
        "END.\n" ;
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
    SymbolTable& symbolTable = functionAst.getSymbolTable() ;
    for(int64_t a = 0 ; a <= 10 ; a++) {
        vector<int64_t >param = {a} ;
        EvaluationContext evaluationContext(param , symbolTable) ;
        optional<int64_t> val = functionAst.evaluate(evaluationContext);
        ASSERT_TRUE(val.has_value());
        ASSERT_EQ(val.value(), a + 1);
    }
}
TEST(TestEvaluation , TestUnary)
{
    constexpr array<string_view , 2> functions = {
        "PARAM a;\n"
        "BEGIN\n"
        "RETURN -a + 1\n"
        "END.\n" ,
        "PARAM a;\n"
        "BEGIN\n"
        "RETURN +a + 1\n"
        "END.\n"
    };
    for(size_t index = 0 ; index < 2 ; index++){
        const auto& code = functions[index] ;
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
        for(int64_t a = 0 ; a <= 10 ; a++) {
            vector<int64_t >param = {a} ;
            SymbolTable symbolTable = functionAst.getSymbolTable() ;
            EvaluationContext evaluationContext(param , symbolTable) ;
            optional<int64_t> val = functionAst.evaluate(evaluationContext);
            ASSERT_TRUE(val.has_value());
            if(index == 0)
                ASSERT_EQ(val.value(), -a + 1);
            else
                ASSERT_EQ(val.value(), +a + 1);

        }

    }
}

TEST(TestEvaluation , TestBinary)
{
    {
        constexpr array<string_view , 4> functions = {
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a + b\n"
            "END.\n" ,
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a - b\n"
            "END.\n" ,
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a * b\n"
            "END.\n" ,
            "PARAM a , b;\n"
            "BEGIN\n"
            "RETURN a / b\n"
            "END.\n" ,
        } ;
        for(size_t index = 0 ; index < 4 ; index ++) {
            const auto& code = functions[index] ;
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
            for (int64_t a = 1; a <= 10; a++)
                for (int64_t b = 1; b <= 10; b++) {
                    vector<int64_t> param = {a , b};
                    SymbolTable symbolTable = functionAst.getSymbolTable();
                    EvaluationContext evaluationContext(param, symbolTable);
                    optional<int64_t> val = functionAst.evaluate(evaluationContext);
                    ASSERT_TRUE(val.has_value());
                    if(index == 0)
                        ASSERT_EQ(val.value(), a + b);
                    else if(index == 1)
                        ASSERT_EQ(val.value(), a - b);
                    else if(index == 2)
                        ASSERT_EQ(val.value(), a * b);
                    else
                        ASSERT_EQ(val.value(), a / b);
                }
        }
    }
    {
        constexpr array<string_view , 3> functions =
        {
                "PARAM a , b;\n"
                "VAR c;\n"
                "CONST d = 15;\n"
                "BEGIN\n"
                "c := d * (a + b);\n"
                "RETURN c\n"
                "END.\n" ,
                "PARAM a , b;\n"
                "VAR c;\n"
                "CONST d = 15;\n"
                "BEGIN\n"
                "c := d * (a - b);\n"
                "RETURN c\n"
                "END.\n" ,
                "PARAM a , b;\n"
                "VAR c;\n"
                "CONST d = 15;\n"
                "BEGIN\n"
                "c := d / (a + b);\n"
                "RETURN c\n"
                "END.\n"
        };
        for(size_t index = 0 ; index < 3 ; index ++) {
            const auto& code = functions[index] ;
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
            for (int64_t a = 1; a <= 10; a++)
                for (int64_t b = 1; b <= 10; b++) {
                    vector<int64_t> param = {a , b};
                    SymbolTable symbolTable = functionAst.getSymbolTable();
                    EvaluationContext evaluationContext(param, symbolTable);
                    optional<int64_t> val = functionAst.evaluate(evaluationContext);
                    ASSERT_TRUE(val.has_value());
                    if(index == 0)
                        ASSERT_EQ(val.value(), 15 * (a + b));
                    else if(index == 1)
                        ASSERT_EQ(val.value(), 15 * (a - b));
                    else
                        ASSERT_EQ(val.value(), 15 / (a + b));
                }
        }
    }
}
TEST(TestEvaluation , TestFunction) {
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

    for(int64_t xa = 0 ; xa <= 4 ; xa ++)
        for(int64_t xb = 0 ; xb <= 4 ; xb ++)
            for(int64_t ya = 0 ; ya <= 4 ; ya ++)
                for(int64_t yb = 0 ; yb <= 4 ; yb ++)
                {
                    vector<int64_t> param = {xa , xb , ya , yb};
                    SymbolTable symbolTable = functionAst.getSymbolTable();
                    EvaluationContext evaluationContext(param, symbolTable);
                    optional<int64_t> val = functionAst.evaluate(evaluationContext);
                    ASSERT_TRUE(val.has_value());
                    int64_t d = (xa - xb) * (xa - xb) + (ya - yb) * (ya - yb) ;
                    ASSERT_EQ(d , val.value()) ;
                }
}
TEST(TestEvaluation , TestDivideByZero) {
    constexpr string_view code =
        "PARAM a;\n"
        "BEGIN\n"
        "RETURN a / 0\n"
        "END.\n" ;
    constexpr string_view expected =
        "3:10: Runtime Error: Divide by Zero\n"
        "RETURN a / 0\n"
        "         ^\n" ;
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
    vector<int64_t> param = {1};
    SymbolTable symbolTable = functionAst.getSymbolTable();
    EvaluationContext evaluationContext(param, symbolTable);
    optional<int64_t> val = functionAst.evaluate(evaluationContext);
    ASSERT_TRUE(!val.has_value());
    ASSERT_EQ(manager.runtimeErrorMessage() , expected) ;
}
