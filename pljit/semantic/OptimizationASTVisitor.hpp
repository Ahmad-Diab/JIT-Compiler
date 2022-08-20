#ifndef PLJIT_OPTIMIZATIONASTVISITOR_HPP
#define PLJIT_OPTIMIZATIONASTVISITOR_HPP

#include "EvaluationContext.hpp"
#include <cstdint>
#include <optional>

//---------------------------------------------------------------------------
namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------

class FunctionAST ;
class ReturnStatementAST ;
class AssignmentStatementAST ;
class BinaryExpressionAST ;
class UnaryExpressionAST ;
class IdentifierAST ;
class LiteralAST ;

class OptimizationVisitor {

    EvaluationContext evaluationContext ;

    public:
    OptimizationVisitor() = default;

    std::optional<int64_t> visitOptimization(FunctionAST& functionAst) ;
    std::optional<int64_t> visitOptimization(ReturnStatementAST& returnStatementAst)  ;
    std::optional<int64_t> visitOptimization(AssignmentStatementAST& assignmentStatementAst)  ;
    std::optional<int64_t> visitOptimization(BinaryExpressionAST& binaryExpressionAst)  ;
    std::optional<int64_t> visitOptimization(UnaryExpressionAST& unaryExpressionAst)  ;
    std::optional<int64_t> visitOptimization(IdentifierAST& identifierAst)  ;
    std::optional<int64_t> visitOptimization(LiteralAST& literalAst)  ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler::semantic
//---------------------------------------------------------------------------
#endif //PLJIT_OPTIMIZATIONASTVISITOR_HPP
