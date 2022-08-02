#ifndef PLJIT_PRINTPARSETREEVISITOR_H
#define PLJIT_PRINTPARSETREEVISITOR_H
//---------------------------------------------------------------------------
#include "ParseTreeVisitor.h"
#include <sstream>
namespace jitcompiler{
//---------------------------------------------------------------------------
class PrintVisitor  : public ParseTreeVisitor{
    private:
    std::ostringstream buf ;
    public:
    void visit(const FunctionDeclaration& functionDeclaration) override ;
    void visit(const ParameterDeclaration& parameterDeclaration) override ;
    void visit(const VariableDeclaration& variableDeclaration) override ;
    void visit(const ConstantDeclaration& constantDeclaration) override;
    void visit(const DeclartorList& declartorList) override ;
    void visit(const InitDeclartorList& initDeclartorList) override;
    void visit(const InitDeclartor& initDeclartor) override ;
    void visit(const CompoundStatement& compoundStatement) override ;
    void visit(const StatementList& statementList) override ;
    void visit(const Statement& statement) override;
    void visit(const AssignmentExpression& assignmentExpression) override ;
    void visit(const AdditiveExpression& additiveExpression) override ;
    void visit(const MultiplicativeExpression& multiplicativeExpression) override;
    void visit(const UnaryExpression& unaryExpression) override ;
    void visit(const PrimaryExpression& primaryExpression) override;
    void visit(const Identifier& identifier) override ;
    void visit(const Literal& literal) override ;
    void visit(const GenericToken& genericToken) override;
    void reset()  ;
    std::string getOutput() const;
};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PRINTPARSETREEVISITOR_H
