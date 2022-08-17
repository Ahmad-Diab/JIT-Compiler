#ifndef PLJIT_PARSETREE_HPP
#define PLJIT_PARSETREE_HPP

//---------------------------------------------------------------------------
#include "CodeReference.hpp"
#include "TokenStream.hpp"
#include <memory>
namespace jitcompiler {
//---------------------------------------------------------------------------
class ParseTreeVisitor ;
//---------------------------------------------------------------------------
/// Base class for ParseTree
class ParseTreeNode {
    protected:
    // TODO codeReference for all non terminal nodes
    //---------------------------------------------------------------------------
     CodeReference codeReference ; // TODO supposed to be in Terminal node only
     CodeManager* codeManager ;
//     TokenStream* tokenStream ;
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

    CodeReference getReference() const ;

    CodeManager* getManager() const ;

    std::string print_dot() const ;
};
class TerminalNode : public ParseTreeNode {

    public:
    explicit TerminalNode(CodeManager* manager) ;
    explicit TerminalNode(CodeManager* manager , CodeReference codeReference) ;

    std::string_view print_token() const;
};
class NonTerminalNode : public ParseTreeNode {
    protected:
    std::vector<std::unique_ptr<ParseTreeNode>> children ;

    public:

    explicit NonTerminalNode(CodeManager* manager)  ;

    const ParseTreeNode & getChild(const size_t index) const;

    std::size_t num_children() const ;

    std::unique_ptr<ParseTreeNode> releaseChild(const size_t index) ;

};
//---------------------------------------------------------------------------
class FunctionDeclaration final : public NonTerminalNode {
    public:

    explicit FunctionDeclaration(CodeManager* manager)  ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class ParameterDeclaration final : public NonTerminalNode {
    public:

    explicit ParameterDeclaration(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class VariableDeclaration final : public NonTerminalNode {
    public:

    explicit VariableDeclaration(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class ConstantDeclaration final : public NonTerminalNode {
    public:

    explicit ConstantDeclaration(CodeManager* manager )   ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class DeclartorList final : public NonTerminalNode {
    public:

    explicit DeclartorList(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class InitDeclartorList final : public NonTerminalNode {
    public:

    explicit InitDeclartorList(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class InitDeclartor final : public NonTerminalNode {
    public:

    explicit InitDeclartor(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class CompoundStatement final : public NonTerminalNode {
    public:

    explicit CompoundStatement(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class StatementList final : public NonTerminalNode {
    public:

    explicit StatementList(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Statement final : public NonTerminalNode {
    public:

    explicit Statement(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class AssignmentExpression final : public NonTerminalNode {
    public:

    explicit AssignmentExpression(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class AdditiveExpression final : public NonTerminalNode {
    public:

    explicit AdditiveExpression(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class MultiplicativeExpression final : public NonTerminalNode {
    public:

    explicit MultiplicativeExpression(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class UnaryExpression final : public NonTerminalNode {
    public:

    explicit UnaryExpression(CodeManager* manager ) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class PrimaryExpression final : public NonTerminalNode {
    public:

    explicit PrimaryExpression(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Identifier final : public TerminalNode {
    public:

    explicit Identifier(CodeManager* manager ) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Literal final : public TerminalNode {
    public:

    explicit Literal(CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class GenericToken final : public TerminalNode {
    public:

    explicit GenericToken(CodeManager* manager , CodeReference codeReference) ;

    Type getType() const override ;

    bool recursiveDecentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PARSETREE_HPP
