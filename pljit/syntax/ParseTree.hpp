#ifndef PLJIT_PARSETREE_HPP
#define PLJIT_PARSETREE_HPP
//---------------------------------------------------------------------------
#include "pljit/syntax/TokenStream.hpp"
#include <memory>
namespace jitcompiler ::syntax{
//---------------------------------------------------------------------------
/// forward declaration to use it in accept() member function
class ParseTreeVisitor ;
class TokenStream ;
//---------------------------------------------------------------------------
/// Base class for ParseTree
class ParseTreeNode {
    protected:
    /// code manager for source code
    management::CodeManager* codeManager ;
    /// node index to label each node for visualization using dot format
    size_t node_index ;
    /// index incrementer to give each node unique identifier number
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
    /// Virtual destructor
    virtual ~ParseTreeNode() ;
    // pure virtual function for type of current subclass
    virtual Type getType() const = 0 ;
    /// accept method using Visitor design pattern
    virtual void accept(ParseTreeVisitor& parseTreeVisitor) const  = 0 ;

    private:
    /// apply recursive descent parser algorithm
    virtual bool recursiveDescentParser(TokenStream& tokenStream) = 0 ;

    public:
    /// get node id
    size_t getNodeId() const ;

    /// compile code using token stream and check if compilation succeed
    bool compileCode(TokenStream& tokenStream) ;

    /// return pointer for code manager
    management::CodeManager* getManager() const ;

    /// print dot format with labels to use it for visualization to display physical graph nodes
    std::string visualizeDot() const ;

    /// print dot format without labels to use it for testing (more readable than visualizeDot()).
    /// nodes are traversed in preorder dfs traversal with corresponding edges
    std::string testDot() const ;

};
class TerminalNode : public ParseTreeNode {
    protected:
    /// code reference for terminal node
    management::CodeReference codeReference ;

    explicit TerminalNode(management::CodeManager* manager) ;
    explicit TerminalNode(management::CodeManager* manager , management::CodeReference codeReference) ;

    public:
    /// print terminal token using codeReference and retrieved from codeManager
    std::string_view print_token() const;

    /// get code reference of terminal token
    management::CodeReference getReference() const ;
};
class NonTerminalNode : public ParseTreeNode {
    protected:
    /// children parse tree node of a non terminal node
    std::vector<std::unique_ptr<ParseTreeNode>> children ;

    explicit NonTerminalNode(management::CodeManager* manager)  ;

    public:
    /// get a child with corresponding index
    const ParseTreeNode & getChild(size_t index) const;

    /// get number of children
    std::size_t num_children() const ;
};
//---------------------------------------------------------------------------
class FunctionDeclaration final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    
    public:
    explicit FunctionDeclaration(management::CodeManager* manager)  ;
    
    Type getType() const override ;
    
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class ParameterDeclaration final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class FunctionDeclaration ; 

    public:
    explicit ParameterDeclaration(management::CodeManager* manager) ;
    Type getType() const override ;
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class VariableDeclaration final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class FunctionDeclaration ; 
    
    public:
    explicit VariableDeclaration(management::CodeManager* manager) ;
    Type getType() const override ;
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class ConstantDeclaration final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class FunctionDeclaration ; 

    public:
    explicit ConstantDeclaration(management::CodeManager* manager )   ;

    Type getType() const override ;
    
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class DeclaratorList final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    
    friend class ParameterDeclaration ; 
    friend class VariableDeclaration ; 
    
    public:

    explicit DeclaratorList(management::CodeManager* manager) ;

    Type getType() const override ;
    
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class InitDeclaratorList final : public NonTerminalNode {
    public:

    explicit InitDeclaratorList(management::CodeManager* manager) ;

    Type getType() const override ;

    bool recursiveDescentParser(TokenStream& tokenStream) override;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;

};
//---------------------------------------------------------------------------
class InitDeclarator final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class InitDeclaratorList ; 
    
    public:
    explicit InitDeclarator(management::CodeManager* manager) ;

    Type getType() const override ;
    
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class CompoundStatement final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class FunctionDeclaration ;
    
    public:
    explicit CompoundStatement(management::CodeManager* manager) ;

    Type getType() const override ;
    
    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class StatementList final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class CompoundStatement ; 

    public:
    explicit StatementList(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Statement final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class StatementList ;

    public:
    explicit Statement(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class AssignmentExpression final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class Statement ;

    public:
    explicit AssignmentExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class AdditiveExpression final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class Statement ;
    friend class AssignmentExpression ;
    friend class PrimaryExpression ;

    public:
    explicit AdditiveExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class MultiplicativeExpression final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class AdditiveExpression ;

    public:
    explicit MultiplicativeExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class UnaryExpression final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class MultiplicativeExpression ;

    public:
    explicit UnaryExpression(management::CodeManager* manager ) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class PrimaryExpression final : public NonTerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class UnaryExpression ;
    public:

    explicit PrimaryExpression(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Identifier final : public TerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class AssignmentExpression ;
    friend class PrimaryExpression ;
    friend class InitDeclarator ;
    friend class DeclaratorList ;

    public:

    explicit Identifier(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class Literal final : public TerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;
    friend class InitDeclarator ;
    friend class PrimaryExpression ;

    public:
    explicit Literal(management::CodeManager* manager) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
class GenericToken final : public TerminalNode {
    private:
    bool recursiveDescentParser(TokenStream& tokenStream) override;

    public:
    explicit GenericToken(management::CodeManager* manager , management::CodeReference codeReference) ;

    Type getType() const override ;

    void accept(ParseTreeVisitor& parseTreeVisitor) const override ;
};
//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------
#endif //PLJIT_PARSETREE_HPP
