#ifndef PLJIT_AST_H
#define PLJIT_AST_H
//---------------------------------------------------------------------------
#include "CodeManager.h"
#include "ParseTree.h"
#include <cassert>
#include <unordered_map>
namespace jitcompiler {
//---------------------------------------------------------------------------
class StatementAST ;
class IdentifierAST ;
class ExpressionAST ;
class ASTVisitor;

class SymbolTable {
    private:
    class DeclarationMap {
        std::unordered_map<std::string_view , CodeReference> declarationReference ;
        std::unordered_map<std::string_view , int64_t> declarationValue ;

        public:

        bool isDeclared(std::string_view identifier) const ;

        bool isDefined (std::string_view identifier) const ;

        void insert(std::string_view  identifier , CodeReference codeReference)  ;

        void set_identifer(std::string_view identifier, int64_t value) ;

        int64_t get_identifier(std::string_view identifier) ;

    };

    DeclarationMap parameterDeclaration ;
    DeclarationMap variableDeclaration ;
    DeclarationMap constantDeclaration ;
    bool isCompiled = true ;

    bool addAttributes(const ParameterDeclaration& declaration) ;
    bool addAttributes(const VariableDeclaration& declaration) ;
    bool addAttributes(const ConstantDeclaration& declaration) ;

    public:

    explicit SymbolTable(const FunctionDeclaration& functionDeclaration) ;

    bool isDeclared(std::string_view identifier) const ;

    bool isDefined(std::string_view identifier) const  ;

    bool isConstant(std::string_view identifier) const ;

    bool isComplied() const ;
};

class ASTNode {
    protected:

        CodeReference codeReference ; // TODO supposed to be in Terminal node only
        CodeManager* codeManager ;
        std::unique_ptr<SymbolTable> symbolTable ;

        size_t node_index ;
        static size_t node_index_incrementer ;

    public:
    enum class Type{
        FUNCTION,
        RETURN_STATEMENT,
        ASSIGNMENT_STATEMENT,
        BINARY_EXPRESSION,
        UNARY_EXPRESSION,
        IDENTIFIER,
        LITERAL
    };

    virtual ~ASTNode() ;

    virtual ASTNode::Type getType() const = 0 ;

    virtual void accept(ASTVisitor& astVistor) const = 0;

    size_t getNodeID() const ;

    CodeReference getReference() const ;


};
class FunctionAST final : public ASTNode {

    std::vector<std::unique_ptr<StatementAST>> children ;

    public:

    explicit FunctionAST(std::unique_ptr<FunctionDeclaration>& functionDeclaration ,CodeManager* manager) ;

    ASTNode::Type getType() const override;

    const StatementAST& getStatement(const size_t index) const ;

    std::size_t statement_size() const ;

    std::unique_ptr<StatementAST> releaseStatement(const size_t index) ;

    void accept(ASTVisitor& astVistor) const override ;

};
class StatementAST : public ASTNode {

    public:

    explicit StatementAST(CodeManager* manager) ;
    explicit StatementAST(CodeManager* manager , CodeReference codeReference1) ;


};
class ExpressionAST  : public ASTNode {

    public:

    explicit ExpressionAST(CodeManager* manager) ;
    explicit ExpressionAST(CodeManager* manager , CodeReference codeReference1) ;
};
class ReturnStatementAST final :public StatementAST {
    std::unique_ptr<ExpressionAST> input ;

    public:
    explicit ReturnStatementAST
        (CodeManager* manager  , std::unique_ptr<ExpressionAST> input) ;

    ASTNode::Type getType() const override;

    const ExpressionAST& getInput() const ;

    std::unique_ptr<ExpressionAST> releaseInput() ;

    void accept(ASTVisitor& astVistor) const override ;

};
class AssignmentStatementAST final : public StatementAST {
    std::unique_ptr<IdentifierAST> leftIdentifier ;
    std::unique_ptr<ExpressionAST> rightExpression ;

    public:
    explicit AssignmentStatementAST(CodeManager* manager);

    explicit AssignmentStatementAST(CodeManager* manager ,
                                    std::unique_ptr<IdentifierAST> left ,
                                    std::unique_ptr<ExpressionAST> right
                                    );

    ASTNode::Type getType() const override;

    const IdentifierAST& getLeftIdentifier() const ;

    std::unique_ptr<IdentifierAST> releaseLeftIdentifier() ;

    const ExpressionAST& getRightExpression() const ;

    std::unique_ptr<ExpressionAST> releaseRightExpression() ;

    void accept(ASTVisitor& astVistor) const override ;

};
class BinaryExpressionAST final : public ExpressionAST {

    std::unique_ptr<ExpressionAST> leftExpression ;
    std::unique_ptr<ExpressionAST> rightExpression ;
    public:
    enum class BinaryType {
        PLUS ,
        MINUS ,
        MULTIPLY,
        DIVIDE
    };
    private:
    BinaryType binaryType ;
    public:
    explicit BinaryExpressionAST
        (CodeManager* manager , BinaryType type , std::unique_ptr<ExpressionAST> left , std::unique_ptr<ExpressionAST> right) ;

    BinaryType getBinaryType() const ;

    ASTNode::Type getType() const override;

    const ExpressionAST& getLeftExpression() const ;

    std::unique_ptr<ExpressionAST> releaseLeftExpression() ;

    const ExpressionAST& getRightExpression() const ;

    std::unique_ptr<ExpressionAST> releaseRightExpression() ;

    void accept(ASTVisitor& astVistor) const override ;

};
class UnaryExpressionAST final : public ExpressionAST {

    std::unique_ptr<ExpressionAST> input ;
    public:
    enum class UnaryType {
        PLUS ,
        MINUS
    };
    private:
    UnaryType unaryType ;
    public:
    explicit UnaryExpressionAST
        (CodeManager* manager  , CodeReference codeReference1 , UnaryType type , std::unique_ptr<ExpressionAST> input) ;

    UnaryType getUnaryType() const ;

    ASTNode::Type getType() const override;

    const ExpressionAST& getInput() const ;

    std::unique_ptr<ExpressionAST> releaseInput() ;

    void accept(ASTVisitor& astVistor) const override ;
};
class IdentifierAST final: public ExpressionAST {

    public:
    explicit IdentifierAST
        (CodeManager* manager , CodeReference codeReference) ;

    ASTNode::Type getType() const override;

    std::string_view print_token() const;

    void accept(ASTVisitor& astVistor) const override ;

};
class LiteralAST final :public ExpressionAST {

    public:
    explicit LiteralAST
        (CodeManager* manager , CodeReference codeReference) ;

    ASTNode::Type getType() const override;

    std::string_view print_token() const;

    void accept(ASTVisitor& astVistor) const override ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_AST_H
