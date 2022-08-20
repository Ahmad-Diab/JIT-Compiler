#ifndef PLJIT_PLJIT_HPP
#define PLJIT_PLJIT_HPP

//---------------------------------------------------------------------------
#include "AST.hpp"
#include "EvaluationContext.hpp"
#include "OptimizationASTVisitor.hpp"
#include "ParseTree.hpp"
#include "TokenStream.hpp"
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string_view>
#include <vector>

namespace jitcompiler {
//---------------------------------------------------------------------------

class Pljit {

    size_t capacity = 0;

    std::vector<std::optional<bool>> compileTrigger ;
    std::vector<std::unique_ptr<std::shared_mutex>> codeMutex ;

    std::vector<std::unique_ptr<CodeManager>> codeManagement ;
    std::vector<std::unique_ptr<TokenStream>> lexicalAnalyzer ;
    std::vector<std::unique_ptr<FunctionDeclaration>> syntaxAnalyzer ;
    std::vector<std::unique_ptr<FunctionAST>> semanticAnalyzer ;
    std::vector<std::unique_ptr<OptimizationVisitor>> optimizer ;

    public:
    auto registerFunction(std::string_view code) {

        size_t index = capacity;
        ++capacity;

        codeMutex.push_back(std::make_unique<std::shared_mutex>()) ;
        compileTrigger.emplace_back(std::nullopt) ;

        std::unique_ptr<CodeManager> codeManager = make_unique<CodeManager>(code) ;

        lexicalAnalyzer.emplace_back(std::make_unique<TokenStream>(codeManager.get())) ;
        syntaxAnalyzer.emplace_back(std::make_unique<FunctionDeclaration>(codeManager.get()))  ;
        semanticAnalyzer.emplace_back(std::make_unique<FunctionAST>(codeManager.get())) ;
        optimizer.emplace_back(std::make_unique<OptimizationVisitor>()) ;
        codeManagement.emplace_back(std::move(codeManager)) ;

        // return pair (value , error_message)
        return [& , index](std::vector<int64_t> parameter_list) -> std::pair<std::optional<int64_t> , std::string> {
            const size_t curIndex = index ;
            std::shared_mutex& mtx = *codeMutex[curIndex] ;
            bool isCompiled ;
            {
                std::unique_lock lock(mtx);
                if (!compileTrigger[curIndex].has_value()) {
                    // uncomment to check on compiling the code for first time only
//                    std::cout << "compileCode\n" ;

                    CodeManager& manager = *codeManagement[curIndex];

                    TokenStream& tokenStream = *lexicalAnalyzer[curIndex] ;
                    tokenStream.compileCode();
                    if (manager.isCodeError()) {
                        compileTrigger[curIndex] = false;
                        return {std::nullopt , manager.error_message()};
                    }

                    FunctionDeclaration& parseTree = *syntaxAnalyzer[curIndex] ;
                    if (!parseTree.compileCode(tokenStream)) {
                        compileTrigger[curIndex] = false;
                        return {std::nullopt , manager.error_message()};
                    }

                    FunctionAST& functionAst = *semanticAnalyzer[curIndex] ;
                    functionAst.compileCode(*syntaxAnalyzer[curIndex]);
                    if (manager.isCodeError()) {
                        compileTrigger[curIndex] = false;
                        return {std::nullopt , manager.error_message()};
                    }

                    OptimizationVisitor&optimizationVisitor = *optimizer[curIndex] ;
                    functionAst.acceptOptimization(optimizationVisitor);

                    compileTrigger[curIndex] = true;

                }
                isCompiled = compileTrigger[curIndex].value() ;
            }
            if(isCompiled) {
                std::optional<int64_t> result ;
                {
                    std::shared_lock lock(mtx);
                    EvaluationContext evaluationContext(parameter_list, semanticAnalyzer[curIndex]->getSymbolTable());
                    result = semanticAnalyzer[curIndex]->evaluate(evaluationContext);
                    if(!result.has_value())
                        return {std::nullopt , codeManagement[curIndex]->runtimeErrorMessage()} ;
                    return {result , ""};
                }
            }
            else {
                {
                    std::shared_lock lock(mtx);
                    return {std::nullopt , codeManagement[curIndex]->error_message()} ;
                }
            }
        } ;
    }

};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PLJIT_HPP
