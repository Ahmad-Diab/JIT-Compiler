#ifndef PLJIT_AST_HPP
#define PLJIT_AST_HPP
//---------------------------------------------------------------------------
#include "pljit/management/CodeManager.hpp"
#include "pljit/syntax/ParseTree.hpp"
#include <cassert>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------
class StatementAST ;
class IdentifierAST ;
class ExpressionAST ;
class ASTVisitor;
class EvaluationContext ;
class OptimizationVisitor ;

class SymbolTable {
    private:
    bool isCompiled = true ;
    management::CodeManager* codeManager ;

    // get<0>(e) = code_ref , get<1>(e) = index in declaration list , get<2>(e) = value (for const declaration only)
    std::array
        <
            std::unordered_map<std::string_view , std::tuple<management::CodeReference , size_t , std::optional<int64_t>>>
        ,
            3
        > tableIdentifier ;
    bool addAttributes(const syntax::ParameterDeclaration& declaration) ;
    bool addAttributes(const syntax::VariableDeclaration& declaration) ;
    bool addAttributes(const syntax::ConstantDeclaration& declaration) ;

    public:
    enum AttributeType {
        PARAMETER,
        VARIABLE,
        CONSTANT
    };

    explicit SymbolTable(management::CodeManager* codeManager , const syntax::FunctionDeclaration& functionDeclaration) ;

    void insert(std::string_view identifier , AttributeType type, management::CodeReference codeReference , size_t index /*for parameter*/, std::optional<int64_t> value) ;

    bool isDeclared(std::string_view identifier) const ;

    bool isConstant(std::string_view identifier)  const;

    bool isVariable(std::string_view identifier) const ;

    bool isComplied() const ;

    std::array<std::unordered_map<std::string_view , std::tuple<management::CodeReference , size_t , std::optional<int64_t>>> , 3> & getTableContent()  ;

};

class ASTNode {
    protected:

        management::CodeReference codeReference ; // TODO supposed to be in Terminal node only
        management::CodeManager* codeManager ;
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

    management::CodeReference getReference() const ;

    SymbolTable& getSymbolTable()  ;
};
class FunctionAST final : public ASTNode {

    std::vector<std::unique_ptr<StatementAST>> children ;
    friend class OptimizationVisitor ;
    public:

    explicit FunctionAST(management::CodeManager* manager) ;

    ASTNode::Type getType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    bool compileCode(const syntax::FunctionDeclaration& functionDeclaration) ;

    const StatementAST& getStatement(size_t index) const ;

    std::size_t statement_size() const ;

    std::unique_ptr<StatementAST> releaseStatement(size_t index) ;


};
class StatementAST : public ASTNode {

    public:
    StatementAST()  ;
    explicit StatementAST(management::CodeManager* manager) ;
    explicit StatementAST(management::CodeManager* manager , management::CodeReference codeReference1) ;

};
class ExpressionAST  : public ASTNode {

    public:
    explicit ExpressionAST() ;
    explicit ExpressionAST(management::CodeManager* manager) ;
    explicit ExpressionAST(management::CodeManager* manager , management::CodeReference codeReference1) ;
};
class ReturnStatementAST final :public StatementAST {
    std::unique_ptr<ExpressionAST> input ;

    friend class OptimizationVisitor ;
    public:
    explicit ReturnStatementAST
        (management::CodeManager* manager  , std::unique_ptr<ExpressionAST> input) ;

    explicit ReturnStatementAST (std::unique_ptr<ExpressionAST> input) : input(std::move(input)){}

    ASTNode::Type getType() const override;

    void accept(ASTVisitor& astVisitor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

    const ExpressionAST& getInput() const ;

    std::unique_ptr<ExpressionAST> releaseInput() ;

};
class AssignmentStatementAST final : public StatementAST {
    std::unique_ptr<IdentifierAST> leftIdentifier ;
    std::unique_ptr<ExpressionAST> rightExpression ;

    friend class OptimizationVisitor ;
    public:
    explicit AssignmentStatementAST(management::CodeManager* manager);

    explicit AssignmentStatementAST(management::CodeManager* manager ,
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
        (management::CodeManager* manager , BinaryType type , std::unique_ptr<ExpressionAST> left , std::unique_ptr<ExpressionAST> right) ;
    explicit BinaryExpressionAST
        (std::unique_ptr<ExpressionAST> left , std::unique_ptr<ExpressionAST> right) : leftExpression(move(left)) , rightExpression(move(right)) {}
    explicit BinaryExpressionAST
        (management::CodeManager* manager , BinaryType type , std::unique_ptr<ExpressionAST> left , std::unique_ptr<ExpressionAST> right , management::CodeReference reference) ;

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
        (management::CodeManager* manager  , management::CodeReference codeReference1 , UnaryType type , std::unique_ptr<ExpressionAST> input) ;
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
        (management::CodeManager* manager , management::CodeReference codeReference) ;

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
        (management::CodeManager* manager , management::CodeReference codeReference) ;

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
#endif //PLJIT_AST_HPP