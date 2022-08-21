#ifndef PLJIT_AST_HPP
#define PLJIT_AST_HPP
//---------------------------------------------------------------------------
#include "pljit/syntax/ParseTree.hpp"
//---------------------------------------------------------------------------
#include <cassert>
#include <optional>
#include <unordered_map>
#include <unordered_set>
//---------------------------------------------------------------------------
namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------
class StatementAST ;
class IdentifierAST ;
class ExpressionAST ;
class ASTVisitor;
class EvaluationContext ;
class OptimizationVisitor ;
//---------------------------------------------------------------------------
class SymbolTable {
    private:
    // check if declarations is compiled correctly
    bool isCompiled = true ;
    management::CodeManager* codeManager{} ;

    // tableIdentifier
    /// array of unordered_map : size = 3 , index=0 -> parameter_ids , index=1 -> variable_ids , index=2 -> constant_ids
    /// unordered_map : key -> identifier -> type(string_view) , value -> tuple(codeRef , index_of declaration list , value of constant declaration)
    /// get<0>(e) = code_ref , get<1>(e) = index in declaration list ,
    /// get<2>(e) = value (for const declaration only)
    std::array
        <
            std::unordered_map<std::string_view , std::tuple<management::CodeReference , size_t , std::optional<int64_t>>>
        ,
            3
        > tableIdentifier ;

    enum AttributeType {
        PARAMETER,
        VARIABLE,
        CONSTANT
    };
    friend class EvaluationContext ;

    /// add attributes of Parameter declarations to table identifier
    bool addAttributes(const syntax::ParameterDeclaration& declaration) ;
    /// add attributes of Variable declarations  to table identifier
    bool addAttributes(const syntax::VariableDeclaration& declaration) ;
    /// add attributes of Constant declarations with corresponding values to table identifier
    bool addAttributes(const syntax::ConstantDeclaration& declaration) ;
    /// insert identifier to table identifier
    void insert(std::string_view identifier , AttributeType type, management::CodeReference codeReference , size_t index /*for parameter*/, std::optional<int64_t> value) ;

    public:
    SymbolTable() ;

    // construct symbol table given parse tree node
    explicit SymbolTable(management::CodeManager* codeManager , const syntax::FunctionDeclaration& functionDeclaration) ;
    // check if identifier is declared
    bool isDeclared(std::string_view identifier) const ;
    //check if identifier is a constant declaration
    bool isConstant(std::string_view identifier)  const;
    //check if identifier is a variable declaration
    bool isVariable(std::string_view identifier) const ;
    // check if source code declarations is compiled
    bool isComplied() const ;
    // get symbol table
    const std::array<std::unordered_map<std::string_view , std::tuple<management::CodeReference , size_t , std::optional<int64_t>>> , 3> & getTableContent()  ;
};

class ASTNode {
    protected:
        // reference for terminal token with represent ASTNode
        management::CodeReference codeReference ;
        // codeManager
        management::CodeManager* codeManager ;
        // SymbolTable
        SymbolTable symbolTable ;
        // node identifier
        size_t node_index ;
        static size_t node_index_incrementer ;

    public:
    enum class ASTType{
        FUNCTION,
        RETURN_STATEMENT,
        ASSIGNMENT_STATEMENT,
        BINARY_EXPRESSION,
        UNARY_EXPRESSION,
        IDENTIFIER,
        LITERAL
    };
    // Virtual Destructor
    virtual ~ASTNode() ;

    // get ASTType of ASTNode
    virtual ASTNode::ASTType getAstType() const = 0 ;

    // used for Visitor design pattern
    virtual void accept(ASTVisitor& astVistor) const = 0;

    // Optimize ASTNode using Visitor design pattern
    virtual std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor) = 0 ;

    // evaluate ASTNode , !has_value() if runtime error is triggered
    virtual std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const = 0 ;

    // get ASTNode unique id
    size_t getNodeID() const ;

    // get symbol table
    SymbolTable& getSymbolTable()  ;

    /// print dot format with labels to use it for visualization to display physical graph nodes
    std::string visualizeDot() const ;

    /// print dot format without labels to use it for testing (more readable than visualizeDot()).
    /// nodes are traversed in preorder dfs traversal with corresponding edges
    std::string testDot() const ;


};
class FunctionAST final : public ASTNode {
    // statements of function node
    std::vector<std::unique_ptr<StatementAST>> children ;
    friend class OptimizationVisitor ;
    public:

    explicit FunctionAST(management::CodeManager* manager) ;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;
    // compile code from FunctionAST node
    bool compileCode(const syntax::FunctionDeclaration& functionDeclaration) ;
    // get statement given certain index
    const StatementAST& getStatement(size_t index) const ;
    // get number of statements
    std::size_t num_statements() const ;
};
class StatementAST : public ASTNode {
    protected:
    StatementAST()  ;
    explicit StatementAST(management::CodeManager* manager) ;
    explicit StatementAST(management::CodeManager* manager , management::CodeReference codeReference1) ;
};
class ExpressionAST  : public ASTNode {
    protected:
    explicit ExpressionAST() ;
    explicit ExpressionAST(management::CodeManager* manager) ;
    explicit ExpressionAST(management::CodeManager* manager , management::CodeReference codeReference1) ;
};
class ReturnStatementAST final :public StatementAST {
    // expression of return statement
    std::unique_ptr<ExpressionAST> input ;
    friend class OptimizationVisitor ;
    public:
    explicit ReturnStatementAST
        (management::CodeManager* manager  , std::unique_ptr<ExpressionAST> input) ;

    explicit ReturnStatementAST (std::unique_ptr<ExpressionAST> input) : input(std::move(input)){}

    // get expression of return statement
    const ExpressionAST& getInput() const ;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVisitor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
class AssignmentStatementAST final : public StatementAST {
    // assigned identifier in assignment statement
    std::unique_ptr<IdentifierAST> leftIdentifier ;
    // expression which is assigned to leftIdentifier
    std::unique_ptr<ExpressionAST> rightExpression ;

    friend class OptimizationVisitor ;
    public:
    explicit AssignmentStatementAST(management::CodeManager* manager);

    explicit AssignmentStatementAST(management::CodeManager* manager ,
                                    std::unique_ptr<IdentifierAST> left ,
                                    std::unique_ptr<ExpressionAST> right
                                    );
    explicit AssignmentStatementAST
        (std::unique_ptr<IdentifierAST> left , std::unique_ptr<ExpressionAST> right) ;
    // get left-side identifier of assignment statement
    const IdentifierAST& getLeftIdentifier() const ;
    // get right-side expression of assignment statement
    const ExpressionAST& getRightExpression() const ;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

};
class BinaryExpressionAST final : public ExpressionAST {
    // left Expression of binary expression
    std::unique_ptr<ExpressionAST> leftExpression ;
    // right Expression of binary expression
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
        (management::CodeManager* manager , BinaryType type , std::unique_ptr<ExpressionAST> left , std::unique_ptr<ExpressionAST> right , management::CodeReference reference) ;

    // get operator of binary expression
    BinaryType getBinaryType() const ;
    // get left-expression of binary expression
    const ExpressionAST& getLeftExpression() const ;
    // get right-expression of binary expression
    const ExpressionAST& getRightExpression() const ;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;

};
class UnaryExpressionAST final : public ExpressionAST {
    // expression of unary expression
    std::unique_ptr<ExpressionAST> input ;
    friend class OptimizationVisitor ;
    public:
    enum class UnaryType {
        PLUS ,
        MINUS
    };
    private:
    // operator of unary expression
    UnaryType unaryType ;
    public:
    explicit UnaryExpressionAST
        (management::CodeManager* manager  , management::CodeReference codeReference1 , UnaryType type , std::unique_ptr<ExpressionAST> input) ;
    // get operator of unary expression
    UnaryType getUnaryType() const ;
    // get expression of unary expression
    const ExpressionAST& getInput() const ;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
class IdentifierAST final: public ExpressionAST {

    public:
    explicit IdentifierAST
        (management::CodeManager* manager , management::CodeReference codeReference) ;

    // print identifier
    std::string_view print_token() const;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
class LiteralAST final :public ExpressionAST {
    // cast literal to signed integer
    int64_t value ;
    friend class OptimizationVisitor ;
    public:
    explicit LiteralAST
        (management::CodeManager* manager , management::CodeReference codeReference) ;

    explicit LiteralAST (int64_t value) ;

    // get value of literal
    int64_t getValue() const ;

    ASTNode::ASTType getAstType() const override;

    void accept(ASTVisitor& astVistor) const override ;

    std::optional<int64_t> evaluate(EvaluationContext& evaluationContext) const override ;

    std::optional<int64_t> acceptOptimization(OptimizationVisitor& astVisitor)  override ;
};
//---------------------------------------------------------------------------
} // namespace jitcompiler::semantic
//---------------------------------------------------------------------------
#endif //PLJIT_AST_HPP