#ifndef PLJIT_OPTIMIZATIONASTVISITOR_HPP
#define PLJIT_OPTIMIZATIONASTVISITOR_HPP

#include "pljit/EvaluationContext.hpp"
#include <cstdint>
#include <optional>

//---------------------------------------------------------------------------
namespace jitcompiler {
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
    explicit OptimizationVisitor(EvaluationContext &evaluationContext) : evaluationContext(evaluationContext) {}

    std::optional<int64_t> visitOptimization(FunctionAST& functionAst) ;
    std::optional<int64_t> visitOptimization(ReturnStatementAST& returnStatementAst)  ;
    std::optional<int64_t> visitOptimization(AssignmentStatementAST& assignmentStatementAst)  ;
    std::optional<int64_t> visitOptimization(BinaryExpressionAST& binaryExpressionAst)  ;
    std::optional<int64_t> visitOptimization(UnaryExpressionAST& unaryExpressionAst)  ;
    std::optional<int64_t> visitOptimization(IdentifierAST& identifierAst)  ;
    std::optional<int64_t> visitOptimization(LiteralAST& literalAst)  ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_OPTIMIZATIONASTVISITOR_HPP
