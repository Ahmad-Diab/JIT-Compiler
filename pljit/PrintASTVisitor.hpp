#ifndef PLJIT_PRINTASTVISITOR_HPP
#define PLJIT_PRINTASTVISITOR_HPP

#include "ASTVisitor.hpp"
#include <sstream>

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

class PrintASTVisitor final : public ASTVisitor {
    private:
    std::ostringstream buf ;
    public:
    void visit(const FunctionAST& functionAst) override ;
    void visit(const ReturnStatementAST& returnStatementAst) override ;
    void visit(const AssignmentStatementAST& assignmentStatementAst) override ;
    void visit(const BinaryExpressionAST& binaryExpressionAst) override ;
    void visit(const UnaryExpressionAST& unaryExpressionAst) override ;
    void visit(const IdentifierAST& identifierAst) override ;
    void visit(const LiteralAST& literalAst) override ;
    void reset()  ;
    std::string getOutput() ;
};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_PRINTASTVISITOR_HPP
