#ifndef PLJIT_AST_H
#define PLJIT_AST_H
//---------------------------------------------------------------------------
#include "pljit/CodeManager.h"
#include "pljit/ParseTree.h"
#include <cassert>
#include <unordered_map>

namespace jitcompiler {
//---------------------------------------------------------------------------
class StatementAST ;
class IdentifierAST ;
class ExpressionAST ;
class ASTVisitor;
class EvaluationContext ;
class OptimizationVisitor ;

class SymbolTable {
    public:
    enum AttributeType {
        PARAMETER,
        VARIABLE,
        CONSTANT
    };
    private:
    bool isCompiled = true ;
    // value-> get<0>(e) = attribute_type , get<1>(e) = code_ref , get<2>(e) = index in declaration list , get<3>(e) = value (for const declaration only)
    std::unordered_map<std::string_view , std::tuple<AttributeType , CodeReference , size_t , std::optional<int64_t>>>
        tableIdentifiers ;

    bool addAttributes(const ParameterDeclaration& declaration) ;
    bool addAttributes(const VariableDeclaration& declaration) ;
    bool addAttributes(const ConstantDeclaration& declaration) ;

    public:
    explicit SymbolTable(const FunctionDeclaration& functionDeclaration) ;

    void insert(std::string_view identifier , AttributeType type, CodeReference codeReference , size_t index , std::optional<int64_t> value) ;

    bool isDeclared(std::string_view identifier) const ;

    bool isConstant(std::string_view identifier)  ;

    bool isComplied() const ;

    const std::unordered_map<std::string_view , std::tuple<AttributeType , CodeReference , size_t , std::optional<int64_t>>> & getTableContent() const ;

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

    virtual std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor) = 0 ;

    virtual std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const = 0 ;

    size_t getNodeID() const ;

    CodeReference getReference() const ;

    const SymbolTable& getSymbolTable() const ;
};
class FunctionAST final : public ASTNode {

    std::vector<std::unique_ptr<StatementAST>> children ;
    friend class OptimizationVisitor ;
    public:

    explicit FunctionAST(std::unique_ptr<FunctionDeclaration>& functionDeclaration ,CodeManager* manager) ;

    ASTNode::Type getType() const override;

    const StatementAST& getStatement(size_t index) const ;

    std::size_t statement_size() const ;

    std::unique_ptr<StatementAST> releaseStatement(size_t index) ;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;


};
class StatementAST : public ASTNode {

    public:
    StatementAST()  ;
    explicit StatementAST(CodeManager* manager) ;
    explicit StatementAST(CodeManager* manager , CodeReference codeReference1) ;

};
class ExpressionAST  : public ASTNode {

    public:
    explicit ExpressionAST() ;
    explicit ExpressionAST(CodeManager* manager) ;
    explicit ExpressionAST(CodeManager* manager , CodeReference codeReference1) ;

};
class ReturnStatementAST final :public StatementAST {
    std::unique_ptr<ExpressionAST> input ;

    friend class OptimizationVisitor ;
    public:
    explicit ReturnStatementAST
        (CodeManager* manager  , std::unique_ptr<ExpressionAST> input) ;

    explicit ReturnStatementAST (std::unique_ptr<ExpressionAST> input) : input(std::move(input)){}

    ASTNode::Type getType() const override;

    const ExpressionAST& getInput() const ;

    std::unique_ptr<ExpressionAST> releaseInput() ;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

};
class AssignmentStatementAST final : public StatementAST {
    std::unique_ptr<IdentifierAST> leftIdentifier ;
    std::unique_ptr<ExpressionAST> rightExpression ;

    friend class OptimizationVisitor ;
    public:
    explicit AssignmentStatementAST(CodeManager* manager);

    explicit AssignmentStatementAST(CodeManager* manager ,
                                    std::unique_ptr<IdentifierAST> left ,
                                    std::unique_ptr<ExpressionAST> right
                                    );
    explicit AssignmentStatementAST
        (std::unique_ptr<IdentifierAST> left , std::unique_ptr<ExpressionAST> right) : leftIdentifier(std::move(left)) , rightExpression(std::move(right)) {}

    ASTNode::Type getType() const override;

    const IdentifierAST& getLeftIdentifier() const ;

    std::unique_ptr<IdentifierAST> releaseLeftIdentifier() ;

    const ExpressionAST& getRightExpression() const ;

    std::unique_ptr<ExpressionAST> releaseRightExpression() ;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

};
class BinaryExpressionAST final : public ExpressionAST {

    std::unique_ptr<ExpressionAST> leftExpression ;
    std::unique_ptr<ExpressionAST> rightExpression ;
    friend class OptimizationVisitor ;
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
    explicit BinaryExpressionAST
        (std::unique_ptr<ExpressionAST> left , std::unique_ptr<ExpressionAST> right) : leftExpression(move(left)) , rightExpression(move(right)) {}

    BinaryType getBinaryType() const ;

    ASTNode::Type getType() const override;

    const ExpressionAST& getLeftExpression() const ;

    std::unique_ptr<ExpressionAST> releaseLeftExpression() ;

    const ExpressionAST& getRightExpression() const ;

    std::unique_ptr<ExpressionAST> releaseRightExpression() ;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

};
class UnaryExpressionAST final : public ExpressionAST {

    std::unique_ptr<ExpressionAST> input ;
    friend class OptimizationVisitor ;
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
    explicit UnaryExpressionAST
        (std::unique_ptr<ExpressionAST> input) : input(std::move(input)) {}

    UnaryType getUnaryType() const ;

    ASTNode::Type getType() const override;

    const ExpressionAST& getInput() const ;

    std::unique_ptr<ExpressionAST> releaseInput() ;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
class IdentifierAST final: public ExpressionAST {

    public:
    explicit IdentifierAST
        (CodeManager* manager , CodeReference codeReference) ;

    ASTNode::Type getType() const override;

    std::string_view print_token() const;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
class LiteralAST final :public ExpressionAST {

    int64_t value ;
    friend class OptimizationVisitor ;
    public:
    explicit LiteralAST
        (CodeManager* manager , CodeReference codeReference) ;
    explicit LiteralAST (int64_t value) ;

    ASTNode::Type getType() const override;

    int64_t getValue() const ;
    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_AST_H
