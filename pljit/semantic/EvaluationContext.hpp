#ifndef PLJIT_EVALUATIONCONTEXT_HPP
#define PLJIT_EVALUATIONCONTEXT_HPP

#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>

//---------------------------------------------------------------------------
namespace jitcompiler::semantic{
//---------------------------------------------------------------------------
class SymbolTable ;

class EvaluationContext {

    std::unordered_map<std::string_view , std::optional<int64_t>> parameters ;
    std::unordered_map<std::string_view , std::optional<int64_t>> variables ;
    std::unordered_map<std::string_view , std::optional<int64_t>> constants ;

    void pushParameter(std::string_view identifier) ;
    void pushVariable(std::string_view identifier) ;
    void pushConstant(std::string_view identifier , int64_t value) ;

    public:
    explicit EvaluationContext() = default ;
    explicit EvaluationContext(SymbolTable & symbolTable) ;
    explicit EvaluationContext(std::vector<int64_t > parameterList ,SymbolTable & symbolTable) ;

    /// update variable or parameter over each assignment statement
    void updateIdentifier(std::string_view identifier, int64_t value) ;
    /// get value of an identifier within evaluation context
    std::optional<int64_t> getIdentifier(std::string_view identifier) ;

};

//---------------------------------------------------------------------------
} // namespace jitcompiler::semantic
//---------------------------------------------------------------------------

#endif //PLJIT_EVALUATIONCONTEXT_HPP
