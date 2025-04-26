#ifndef PLJIT_PLJIT_HPP
#define PLJIT_PLJIT_HPP
//---------------------------------------------------------------------------
#include "pljit/semantic/AST.hpp"
#include "pljit/semantic/OptimizationASTVisitor.hpp"
//---------------------------------------------------------------------------
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string_view>
#include <vector>
//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

class Pljit {
    // number of registered functions
    size_t capacity = 0;

    // for each function check if code not compiled (nullopt) , compilation success(true) , compilation failure (false)
    std::vector<std::optional<bool>> compileTrigger ;
    // mutex for each function
    std::vector<std::unique_ptr<std::shared_mutex>> codeMutex ;
    // code manager for source code of each function
    std::vector<std::unique_ptr<management::CodeManager>> codeManagement ;
    // token stream for each function
    std::vector<std::unique_ptr<syntax::TokenStream>> lexicalAnalyzer ;
    // parse tree node for each function
    std::vector<std::unique_ptr<syntax::FunctionDeclaration>> syntaxAnalyzer ;
    // AST node for each function
    std::vector<std::unique_ptr<semantic::FunctionAST>> semanticAnalyzer ;
    // AST Optimizer for each function
    std::vector<std::unique_ptr<semantic::OptimizationVisitor>> optimizer ;

    public:
    auto registerFunction(std::string_view code) {

        // index for current registered function
        size_t index = capacity;
        ++capacity;

        // initialize all resources of registered function (without any compilation of code)

        codeMutex.push_back(std::make_unique<std::shared_mutex>()) ;
        compileTrigger.emplace_back(std::nullopt) ;

        std::unique_ptr<management::CodeManager> codeManager = make_unique<management::CodeManager>(code) ;

        lexicalAnalyzer.emplace_back(std::make_unique<syntax::TokenStream>(codeManager.get())) ;
        syntaxAnalyzer.emplace_back(std::make_unique<syntax::FunctionDeclaration>(codeManager.get()))  ;
        semanticAnalyzer.emplace_back(std::make_unique<semantic::FunctionAST>(codeManager.get())) ;
        optimizer.emplace_back(std::make_unique<semantic::OptimizationVisitor>()) ;
        codeManagement.emplace_back(std::move(codeManager)) ;

        //lambda function for compiling code and calling registered function each time with different parameters
        /// return pair (value , error_message)
        /// capture all declaration by reference except index by value

        return [& , index](std::vector<int64_t> parameter_list) -> std::pair<std::optional<int64_t> /*value*/ , std::string /*error message*/> {
            // assume user will add correct number of parameters => will not trigger an error

            std::shared_mutex& mtx = *codeMutex[index] ; // synchronized shared resources
            bool isCompiled ;
            {
                std::unique_lock lock(mtx);
                if (!compileTrigger[index].has_value()) {
                    // uncomment to check if it is compiled for first time only
//                    std::cout << "compileCode\n" ;

                    management::CodeManager& manager = *codeManagement[index];
                    syntax::TokenStream& tokenStream = *lexicalAnalyzer[index] ;

                    if (!tokenStream.compileCode()) {
                        assert(!manager.error_message().empty()) ;
                        compileTrigger[index] = false;
                        return {std::nullopt , manager.error_message()};
                    }
                    assert(manager.error_message().empty()) ;

                    syntax::FunctionDeclaration& parseTree = *syntaxAnalyzer[index] ;
                    if (!parseTree.compileCode(tokenStream)) {
                        assert(!manager.error_message().empty()) ;
                        compileTrigger[index] = false;
                        return {std::nullopt , manager.error_message()};
                    }
                    assert(manager.error_message().empty()) ;

                    semantic::FunctionAST& functionAst = *semanticAnalyzer[index] ;
                    if (!functionAst.compileCode(*syntaxAnalyzer[index])) {
                        assert(!manager.error_message().empty()) ;
                        compileTrigger[index] = false;
                        return {std::nullopt , manager.error_message()};
                    }
                    assert(manager.error_message().empty()) ;

                    functionAst.acceptOptimization(*optimizer[index]);

                    compileTrigger[index] = true;
                }
                isCompiled = compileTrigger[index].value() ;
            }
            if(isCompiled) {
                std::optional<int64_t> result ;
                {
                    std::shared_lock lock(mtx);
                    semantic::EvaluationContext evaluationContext(std::move(parameter_list), semanticAnalyzer[index]->getSymbolTable());
                    result = semanticAnalyzer[index]->evaluate(evaluationContext);
                    if (!result.has_value()) {
                        // runtimeErrorMessage will be cleared immediately from output stream
                        return {std::nullopt, codeManagement[index]->runtimeErrorMessage()};
                    }
                }
                return {result , ""};
            }
            else {
                std::string errorMessage  ;
                {
                    std::shared_lock lock(mtx);
                    errorMessage = codeManagement[index]->error_message();
                }
                assert(!errorMessage.empty()) ;
                return {std::nullopt , errorMessage} ;
            }
        } ;
    }
};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PLJIT_HPP
