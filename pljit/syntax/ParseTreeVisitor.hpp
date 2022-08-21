#ifndef PLJIT_PARSETREEVISITOR_HPP
#define PLJIT_PARSETREEVISITOR_HPP

//---------------------------------------------------------------------------
namespace jitcompiler ::syntax{
//---------------------------------------------------------------------------

/// forward-declarations
class FunctionDeclaration ;
class ParameterDeclaration ;
class VariableDeclaration ;
class ConstantDeclaration ;
class DeclaratorList ;
class InitDeclaratorList ;
class InitDeclarator ;
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
        virtual ~ParseTreeVisitor() = default ;
        //---------------------------------------------------------------------------
        virtual void visit(const FunctionDeclaration& functionDeclaration) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const ParameterDeclaration& parameterDeclaration) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const VariableDeclaration& variableDeclaration) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const ConstantDeclaration& constantDeclaration) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const DeclaratorList& declaratorList) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const InitDeclaratorList& initDeclaratorList) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const InitDeclarator& initDeclarator) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const CompoundStatement& compoundStatement) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const StatementList& statementList) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const Statement& statement) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const AssignmentExpression& assignmentExpression) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const AdditiveExpression& additiveExpression) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const MultiplicativeExpression& multiplicativeExpression) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const UnaryExpression& unaryExpression) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const PrimaryExpression& primaryExpression) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const Identifier& identifier) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const Literal& literal) = 0 ;
        //---------------------------------------------------------------------------
        virtual void visit(const GenericToken& genericToken) = 0 ;
        //---------------------------------------------------------------------------
};
//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------

#endif //PLJIT_PARSETREEVISITOR_HPP
