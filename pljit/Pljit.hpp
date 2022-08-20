#ifndef PLJIT_PLJIT_HPP
#define PLJIT_PLJIT_HPP

//---------------------------------------------------------------------------
#include "pljit/semantic/AST.hpp"
#include "pljit/semantic/EvaluationContext.hpp"
#include "pljit/semantic/OptimizationASTVisitor.hpp"
#include "pljit/syntax/ParseTree.hpp"
#include "pljit/syntax/TokenStream.hpp"
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

    std::vector<std::unique_ptr<management::CodeManager>> codeManagement ;
    std::vector<std::unique_ptr<syntax::TokenStream>> lexicalAnalyzer ;
    std::vector<std::unique_ptr<syntax::FunctionDeclaration>> syntaxAnalyzer ;
    std::vector<std::unique_ptr<semantic::FunctionAST>> semanticAnalyzer ;
    std::vector<std::unique_ptr<semantic::OptimizationVisitor>> optimizer ;

    public:
    auto registerFunction(std::string_view code) {

        size_t index = capacity;
        ++capacity;

        codeMutex.push_back(std::make_unique<std::shared_mutex>()) ;
        compileTrigger.emplace_back(std::nullopt) ;

        std::unique_ptr<management::CodeManager> codeManager = make_unique<management::CodeManager>(code) ;

        lexicalAnalyzer.emplace_back(std::make_unique<syntax::TokenStream>(codeManager.get())) ;
        syntaxAnalyzer.emplace_back(std::make_unique<syntax::FunctionDeclaration>(codeManager.get()))  ;
        semanticAnalyzer.emplace_back(std::make_unique<semantic::FunctionAST>(codeManager.get())) ;
        optimizer.emplace_back(std::make_unique<semantic::OptimizationVisitor>()) ;
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

                    management::CodeManager& manager = *codeManagement[curIndex];

                    syntax::TokenStream& tokenStream = *lexicalAnalyzer[curIndex] ;
                    tokenStream.compileCode();
                    if (manager.isCodeError()) {
                        compileTrigger[curIndex] = false;
                        return {std::nullopt , manager.error_message()};
                    }

                    syntax::FunctionDeclaration& parseTree = *syntaxAnalyzer[curIndex] ;
                    if (!parseTree.compileCode(tokenStream)) {
                        compileTrigger[curIndex] = false;
                        return {std::nullopt , manager.error_message()};
                    }

                    semantic::FunctionAST& functionAst = *semanticAnalyzer[curIndex] ;
                    functionAst.compileCode(*syntaxAnalyzer[curIndex]);
                    if (manager.isCodeError()) {
                        compileTrigger[curIndex] = false;
                        return {std::nullopt , manager.error_message()};
                    }

                    semantic::OptimizationVisitor&optimizationVisitor = *optimizer[curIndex] ;
                    functionAst.acceptOptimization(optimizationVisitor);

                    compileTrigger[curIndex] = true;

                }
                isCompiled = compileTrigger[curIndex].value() ;
            }
            if(isCompiled) {
                std::optional<int64_t> result ;
                {
                    std::shared_lock lock(mtx);
                    semantic::EvaluationContext evaluationContext(parameter_list, semanticAnalyzer[curIndex]->getSymbolTable());
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
