#ifndef PLJIT_EVALUATIONCONTEXT_H
#define PLJIT_EVALUATIONCONTEXT_H

#include <string_view>
#include <unordered_map>
#include <vector>

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------
class SymbolTable ;

class EvaluationContext {
    std::unordered_map<std::string_view , int64_t> parameters ;
    std::unordered_map<std::string_view , int64_t> variables ;
    std::unordered_map<std::string_view , int64_t> constants ;

    void pushParameter(std::string_view identifier , int64_t value) ;
    void pushConstant(std::string_view identifier , int64_t value) ;

    public:
    explicit EvaluationContext(const std::vector<int64_t >& parameterList , const SymbolTable & symbolTable) ;
    explicit EvaluationContext(const SymbolTable & symbolTable) ;


    void updateIdentifier(std::string_view identifier, int64_t value) ;

    std::optional<int64_t> getIdentifier(std::string_view identifier) ;

};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_EVALUATIONCONTEXT_H
