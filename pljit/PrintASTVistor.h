#ifndef PLJIT_PRINTASTVISTOR_H
#define PLJIT_PRINTASTVISTOR_H

#include "ASTVisitor.h"
#include <sstream>

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

class PrintASTVistor final : public ASTVisitor {
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
    std::string getOutput() const;

};



//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_PRINTASTVISTOR_H
