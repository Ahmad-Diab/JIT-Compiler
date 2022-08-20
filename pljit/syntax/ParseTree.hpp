#ifndef PLJIT_PARSETREE_HPP
#define PLJIT_PARSETREE_HPP

//---------------------------------------------------------------------------
#include "TokenStream.hpp"
#include "pljit/management/CodeReference.hpp"
#include <memory>
namespace jitcompiler ::syntax{
//---------------------------------------------------------------------------
class ParseTreeVisitor ;
//---------------------------------------------------------------------------
/// Base class for ParseTree
class ParseTreeNode {
    protected:
    // TODO codeReference for all non terminal nodes
    //---------------------------------------------------------------------------
     management::CodeReference codeReference ;
     management::CodeManager* codeManager ;
     //---------------------------------------------------------------------------

     size_t node_index ;
     static size_t node_index_incrementer ;
public:
    /// Types of ParseTreeNodes
    enum class Type {
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

    virtual bool recursiveDecentParser(TokenStream& tokenStream) = 0 ;

    virtual void accept(ParseTreeVisitor& parseTreeVisitor) const  = 0 ;

    size_t getNodeId() const ;

    bool compileCode(TokenStream& tokenStream) ;

    management::CodeReference getReference() const ;

    management::CodeManager* getManager() const ;

    std::string visualizeDot() const ;
};
class TerminalNode : public ParseTreeNode {

    public:
    explicit TerminalNode(management::CodeManager* manager) ;
    explicit TerminalNode(management::CodeManager* manager , management::CodeReference codeReference) ;

    std::string_view print_token() const;
};
class NonTerminalNode : public ParseTreeNode {
    protected:
    std::vector<std::unique_ptr<ParseTreeNode>> children ;

    public:

    explicit NonTerminalNode(management::CodeManager* manager)  ;

    const ParseTreeNode & getChild(size_t index) const;

    std::size_t num_children() const ;

    std::unique_ptr<ParseTreeNode> releaseChild(size_t index) ;

};
//---------------------------------------------------------------------------
class FunctionDeclaration final : public NonTerminalNode {
    public:

    explicit FunctionDeclaration(management::CodeManager* manager)  ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class ParameterDeclaration final : public NonTerminalNode {
    public:

    explicit ParameterDeclaration(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class VariableDeclaration final : public NonTerminalNode {
    public:

    explicit VariableDeclaration(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class ConstantDeclaration final : public NonTerminalNode {
    public:

    explicit ConstantDeclaration(management::CodeManager* manager )   ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class DeclartorList final : public NonTerminalNode {
    public:

    explicit DeclartorList(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class InitDeclartorList final : public NonTerminalNode {
    public:

    explicit InitDeclartorList(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class InitDeclartor final : public NonTerminalNode {
    public:

    explicit InitDeclartor(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class CompoundStatement final : public NonTerminalNode {
    public:

    explicit CompoundStatement(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class StatementList final : public NonTerminalNode {
    public:

    explicit StatementList(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Statement final : public NonTerminalNode {
    public:

    explicit Statement(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class AssignmentExpression final : public NonTerminalNode {
    public:

    explicit AssignmentExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class AdditiveExpression final : public NonTerminalNode {
    public:

    explicit AdditiveExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class MultiplicativeExpression final : public NonTerminalNode {
    public:

    explicit MultiplicativeExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class UnaryExpression final : public NonTerminalNode {
    public:

    explicit UnaryExpression(management::CodeManager* manager ) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class PrimaryExpression final : public NonTerminalNode {
    public:

    explicit PrimaryExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Identifier final : public TerminalNode {
    public:

    explicit Identifier(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Literal final : public TerminalNode {
    public:

    explicit Literal(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class GenericToken final : public TerminalNode {
    public:

    explicit GenericToken(management::CodeManager* manager , management::CodeReference codeReference) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------
#endif //PLJIT_PARSETREE_HPP
