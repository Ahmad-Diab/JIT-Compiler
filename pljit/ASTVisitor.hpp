#ifndef PLJIT_ASTVISITOR_HPP
#define PLJIT_ASTVISITOR_HPP

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

class ASTVisitor {
    public:
    virtual void visit(const FunctionAST& functionAst) = 0 ;
    virtual void visit(const ReturnStatementAST& returnStatementAst) = 0 ;
    virtual void visit(const AssignmentStatementAST& assignmentStatementAst) = 0 ;
    virtual void visit(const BinaryExpressionAST& binaryExpressionAst) = 0 ;
    virtual void visit(const UnaryExpressionAST& unaryExpressionAst) = 0 ;
    virtual void visit(const IdentifierAST& identifierAst) = 0 ;
    virtual void visit(const LiteralAST& literalAst) = 0 ;
};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_ASTVISITOR_HPP
