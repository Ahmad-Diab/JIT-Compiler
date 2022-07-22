#ifndef PLJIT_PARSETREE_H
#define PLJIT_PARSETREE_H

//---------------------------------------------------------------------------
#include "pljit/lib/TokenStream.h"
#include "pljit/lib/CodeReference.h"
#include <memory>
namespace jitcompiler {
//---------------------------------------------------------------------------
/// Base class for ParseTree
class ParseTreeNode {
    protected:
    // TODO codeReference for all non terminal nodes
     CodeReference codeReference ; // will be constructed
     CodeManager* codeManager ;
     TokenStream* tokenStream ;
     bool isCompileError ;
public:
    /// Types of ParseTreeNodes
    enum class Type{
        /// Declaration types
        FUNCTION_DECLARATION ,
        PARAMETER_DECLARATION ,
        VARIABLE_DECLARATION ,
        CONSTANT_DECLARATION ,
        DECLARATOR_LIST ,
        INIT_DECLARATOR_LIST ,
        INIT_DECLARATOR ,
        /// Statement types;
        COMPOUND_STATEMENT,
        STATEMENT_LIST,
        STATEMENT,
        /// Expressions
        ASSIGNMENT_EXPRESSION,
        ADDITIVE_EXPRESSION,
        MULTIPLICATIVE_EXPRESSION,
        UNARY_EXPRESSION,
        PRIMARY_EXPRESSION,
        /// Terminal types
        IDENTIFIER,
        LITERAL,
        GENERIC_TOKEN
    };
//    // TODO check for correctness
    virtual ~ParseTreeNode() ;
    // pure virtual function for type of current subclass
    virtual Type getType() const = 0 ;

    virtual bool recursiveDecentParser() = 0 ;

    virtual bool isInitialized() const ;
};
class TerminalNode : public ParseTreeNode {

    public:
    explicit TerminalNode(CodeManager* manager , TokenStream* tokenStream) ;
    explicit TerminalNode(CodeReference codeReference) ;

};
class NonTerminalNode : public ParseTreeNode {
    protected:
    std::vector<std::unique_ptr<ParseTreeNode>> children ;

    public:

    explicit NonTerminalNode(CodeManager* manager , TokenStream* tokenStream)  ;

    virtual const ParseTreeNode & getChild(const size_t index) const;

    virtual std::size_t num_children() const ;

    virtual std::unique_ptr<ParseTreeNode> releaseChild(const size_t index) ;

};
//---------------------------------------------------------------------------
class FunctionDeclaration final : public NonTerminalNode {
    public:
    explicit FunctionDeclaration(CodeManager* manager , TokenStream* tokenStream)  ;

//    explicit FunctionDeclaration(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class ParameterDeclaration final : public NonTerminalNode {
    public:
    explicit ParameterDeclaration(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit ParameterDeclaration(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class VariableDeclaration final : public NonTerminalNode {
    public:
    explicit VariableDeclaration(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit VariableDeclaration(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class ConstantDeclaration final : public NonTerminalNode {
    public:
    explicit ConstantDeclaration(CodeManager* manager , TokenStream* tokenStream)   ;

//    explicit ConstantDeclaration(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class DeclartorList final : public NonTerminalNode {
    public:
    explicit DeclartorList(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit DeclartorList(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class InitDeclartorList final : public NonTerminalNode {
    public:
    explicit InitDeclartorList(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit InitDeclartorList(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class InitDeclartor final : public NonTerminalNode {
    public:
    explicit InitDeclartor(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit InitDeclartor(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class CompoundStatement final : public NonTerminalNode {
    public:
    explicit CompoundStatement(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit CompoundStatement(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class StatementList final : public NonTerminalNode {
    public:
    explicit StatementList(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit StatementList(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class Statement final : public NonTerminalNode {
    public:
    explicit Statement(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit Statement(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class AssignmentExpression final : public NonTerminalNode {
    public:
    explicit AssignmentExpression(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit AssignmentExpression(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class AdditiveExpression final : public NonTerminalNode {
    public:
    explicit AdditiveExpression(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit AdditiveExpression(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class MultiplicativeExpression final : public NonTerminalNode {
    public:
    explicit MultiplicativeExpression(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit MultiplicativeExpression(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class UnaryExpression final : public NonTerminalNode {
    public:
    explicit UnaryExpression(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit UnaryExpression(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class PrimaryExpression final : public NonTerminalNode {
    public:
    explicit PrimaryExpression(CodeManager* manager , TokenStream* tokenStream) ;

//    explicit PrimaryExpression(CodeManager* manager , std::vector<std::unique_ptr<ParseTreeNode>> list) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class Identifier final : public TerminalNode {
    public:
    explicit Identifier(CodeManager* manager , TokenStream* tokenStream) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class Literal final : public TerminalNode {
    public:
    explicit Literal(CodeManager* manager , TokenStream* tokenStream) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
class GenericToken final : public TerminalNode {
    public:
    explicit GenericToken(CodeReference codeReference) ;

    Type getType() const override ;

    bool recursiveDecentParser() override;
};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PARSETREE_H
