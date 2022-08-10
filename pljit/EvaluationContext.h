#ifndef PLJIT_EVALUATIONCONTEXT_H
#define PLJIT_EVALUATIONCONTEXT_H

#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------
class SymbolTable ;

class EvaluationContext {

    SymbolTable* symbolTable;
    std::unordered_map<std::string_view , std::optional<int64_t>> parameters ;
    std::unordered_map<std::string_view , std::optional<int64_t>> variables ;
    std::unordered_map<std::string_view , std::optional<int64_t>> constants ;

    void pushParameter(std::string_view identifier) ;
    void pushVariable(std::string_view identifier) ;
    void pushConstant(std::string_view identifier , int64_t value) ;

    public:
    explicit EvaluationContext() = default ;
    explicit EvaluationContext(SymbolTable & symbolTable) ;
    explicit EvaluationContext(std::vector<int64_t >& parameterList , SymbolTable & symbolTable) ;

    void updateIdentifier(std::string_view identifier, int64_t value) ;

    std::optional<int64_t> getIdentifier(std::string_view identifier) ;

};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_EVALUATIONCONTEXT_H
