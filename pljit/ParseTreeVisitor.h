#ifndef PLJIT_PARSETREEVISITOR_H
#define PLJIT_PARSETREEVISITOR_H

//---------------------------------------------------------------------------
namespace jitcompiler{
//---------------------------------------------------------------------------

class FunctionDeclaration ;
class ParameterDeclaration ;
class VariableDeclaration ;
class ConstantDeclaration ;
class DeclartorList ;
class InitDeclartorList ;
class InitDeclartor ;
class CompoundStatement ;
class StatementList ;
class Statement ;
class AssignmentExpression ;
class AdditiveExpression ;
class MultiplicativeExpression ;
class UnaryExpression ;
class PrimaryExpression ;
class Identifier ;
class Literal ;
class GenericToken ;

class ParseTreeVisitor{
    public:
        virtual void visit(const FunctionDeclaration& functionDeclaration) = 0 ;
        virtual void visit(const ParameterDeclaration& parameterDeclaration) = 0 ;
        virtual void visit(const VariableDeclaration& variableDeclaration) = 0 ;
        virtual void visit(const ConstantDeclaration& constantDeclaration) = 0 ;
        virtual void visit(const DeclartorList& declartorList) = 0 ;
        virtual void visit(const InitDeclartorList& initDeclartorList) = 0 ;
        virtual void visit(const InitDeclartor& initDeclartor) = 0 ;
        virtual void visit(const CompoundStatement& compoundStatement) = 0 ;
        virtual void visit(const StatementList& statementList) = 0 ;
        virtual void visit(const Statement& statement) = 0 ;
        virtual void visit(const AssignmentExpression& assignmentExpression) = 0 ;
        virtual void visit(const AdditiveExpression& additiveExpression) = 0 ;
        virtual void visit(const MultiplicativeExpression& multiplicativeExpression) = 0 ;
        virtual void visit(const UnaryExpression& unaryExpression) = 0 ;
        virtual void visit(const PrimaryExpression& primaryExpression) = 0 ;
        virtual void visit(const Identifier& identifier) = 0 ;
        virtual void visit(const Literal& literal) = 0 ;
        virtual void visit(const GenericToken& genericToken) = 0 ;
};


//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_PARSETREEVISITOR_H
