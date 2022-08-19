#ifndef PLJIT_PLJIT_HPP
#define PLJIT_PLJIT_HPP

//---------------------------------------------------------------------------
#include "AST.hpp"
#include "ParseTree.hpp"
#include "TokenStream.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>
namespace jitcompiler {
//---------------------------------------------------------------------------

class Pljit {

    size_t capacity = 0;
    std::vector<std::optional<bool>> compileTrigger ;
    std::vector<std::unique_ptr<CodeManager>> codeManagement ;
    std::vector<std::unique_ptr<TokenStream>> lexicalAnalyzer ;
    std::vector<std::unique_ptr<FunctionDeclaration>> syntaxAnalyzer ;
    std::vector<std::unique_ptr<FunctionAST>> semanticAnalyzer ;

    public:
    auto registerFunction(std::string_view code) {

        size_t index = capacity;
        ++capacity;

        codeManagement.emplace_back(make_unique<CodeManager>(code)) ;
        lexicalAnalyzer.emplace_back() ;
        syntaxAnalyzer.emplace_back()  ;
        semanticAnalyzer.emplace_back() ;
        compileTrigger.emplace_back(std::nullopt) ;

        return [& , index](std::vector<int64_t>& parameter_list) -> std::optional<int64_t> {
            const size_t curIndex = index ;
            if(!compileTrigger[curIndex].has_value()) {
                std::cout << "compile" << '\n';
                CodeManager* manager = codeManagement[curIndex].get() ;
                lexicalAnalyzer[curIndex] = std::make_unique<TokenStream>(manager);
                lexicalAnalyzer[curIndex]->compileCode() ;
                if(manager->isCodeError()) {
                    compileTrigger[curIndex] = false ;
                    std::cout << manager->error_message() << '\n';
                    return std::nullopt ;
                }
                auto& tokenStream = lexicalAnalyzer[curIndex] ;
                syntaxAnalyzer[curIndex] = std::make_unique<FunctionDeclaration>(manager) ;
                if(!syntaxAnalyzer[curIndex]->compileCode(*tokenStream)) {
                    compileTrigger[curIndex] = false ;
                    std::cout << manager->error_message() << '\n';
                    return std::nullopt ;
                }
                semanticAnalyzer[curIndex] = std::make_unique<FunctionAST>(manager) ;
                semanticAnalyzer[curIndex]->compileCode(*syntaxAnalyzer[curIndex]) ;
                if(manager->isCodeError()) {
                    compileTrigger[curIndex] = false ;
                    std::cout << manager->error_message() << '\n' ;
                    return std::nullopt ;
                }
                FunctionAST* functionAst = semanticAnalyzer[curIndex].get() ;
                EvaluationContext evalOptimization(semanticAnalyzer[curIndex]->getSymbolTable()) ;
                OptimizationVisitor optimizationVisitor(evalOptimization) ;
                functionAst->acceptOptimization(optimizationVisitor) ;
                compileTrigger[curIndex] = true ;
                EvaluationContext evaluationContext(parameter_list , semanticAnalyzer[curIndex]->getSymbolTable()) ;
                std::optional<int64_t> result = semanticAnalyzer[curIndex]->evaluate(evaluationContext) ;
                // TODO runtime error divide by zero.
                assert(result.has_value()) ;
                return result ;
            }
            else if(compileTrigger[curIndex].value()) {

                EvaluationContext evaluationContext(parameter_list , semanticAnalyzer[curIndex]->getSymbolTable()) ;
                std::optional<int64_t> result = semanticAnalyzer[curIndex]->evaluate(evaluationContext) ;
                // TODO runtime error divide by zero.
                assert(result.has_value()) ;
                return result ;
            }
            else {
                std::cout << codeManagement[curIndex]->error_message() << '\n' ;
                return std::nullopt ;
            }
        } ;
    }

};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PLJIT_HPP
