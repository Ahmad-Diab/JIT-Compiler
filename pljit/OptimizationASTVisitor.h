#ifndef PLJIT_OPTIMIZATIONASTVISITOR_H
#define PLJIT_OPTIMIZATIONASTVISITOR_H

#include "ASTVisitor.h"

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

class OptimizationASTVisitor final : public ASTVisitor{

    public:
    void visit(const FunctionAST& functionAst) override ;
    void visit(const ReturnStatementAST& returnStatementAst) override ;
    void visit(const AssignmentStatementAST& assignmentStatementAst) override ;
    void visit(const BinaryExpressionAST& binaryExpressionAst) override ;
    void visit(const UnaryExpressionAST& unaryExpressionAst) override ;
    void visit(const IdentifierAST& identifierAst) override ;
    void visit(const LiteralAST& literalAst) override ;


};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_OPTIMIZATIONASTVISITOR_H
