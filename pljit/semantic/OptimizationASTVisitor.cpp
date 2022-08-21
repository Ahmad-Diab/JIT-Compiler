#include "pljit/semantic/OptimizationASTVisitor.hpp"
#include "pljit/semantic/AST.hpp"
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------
optional<int64_t> OptimizationVisitor::visitOptimization(FunctionAST& functionAst) {
    // initialize evaluation context starting from function ast
    evaluationContext = EvaluationContext(functionAst.getSymbolTable()) ;

    for(size_t statement_index = 0 ; statement_index < functionAst.num_statements() ; statement_index++)
    {
        StatementAST& statementAst = *functionAst.children[statement_index] ;
        optional<int64_t> result = statementAst.acceptOptimization(*this) ;
        if(statementAst.getAstType() == ASTNode::ASTType::RETURN_STATEMENT) {
            if(result)
            // if it returns constant then return literal with evaluated value
            {
                functionAst.children.clear();
                unique_ptr<ReturnStatementAST> retStatement = make_unique<ReturnStatementAST>(make_unique<LiteralAST>(result.value())) ;
                functionAst.children.emplace_back(move(retStatement)) ;
                return result ;
            }
            else
            {
                if(statement_index + 1 < functionAst.num_statements())
                    // dead code elimination
                   functionAst.children.resize(statement_index + 1) ;
            }
        }
        else {
            if(result)
            // if right expression is constant then right identifier should be assigned to const val in runtime
            {
                AssignmentStatementAST& assignmentStatementAst = static_cast<AssignmentStatementAST&>(statementAst);
                string_view identifier = assignmentStatementAst.getLeftIdentifier().print_token() ;
                assignmentStatementAst.rightExpression = make_unique<LiteralAST>(result.value()) ;
                // update it if there are more optimizations in next statements
                evaluationContext.updateIdentifier(identifier , result.value()) ;
            }
        }
    }
    return nullopt;
}
//---------------------------------------------------------------------------
std::optional<int64_t> OptimizationVisitor::visitOptimization(ReturnStatementAST& returnStatementAst) {
    return returnStatementAst.input->acceptOptimization(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> OptimizationVisitor::visitOptimization(AssignmentStatementAST& assignmentStatementAst) {
    return assignmentStatementAst.rightExpression->acceptOptimization(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> OptimizationVisitor::visitOptimization(BinaryExpressionAST& binaryExpressionAst) {
    //optimize left and right expressions recursively
    auto leftResult = binaryExpressionAst.leftExpression->acceptOptimization(*this) ;
    auto rightResult = binaryExpressionAst.rightExpression->acceptOptimization(*this) ;

    // change left expression to a constant value if leftResult is evaluated
    if(leftResult)
        binaryExpressionAst.leftExpression = make_unique<LiteralAST>(leftResult.value()) ;
    // change right expression to a constant value if rightResult is evaluated
    if(rightResult)
        binaryExpressionAst.rightExpression = make_unique<LiteralAST>(rightResult.value()) ;
    // return evaluated binary expression if left & right expressions become constants
    if(leftResult && rightResult)
    {
        switch (binaryExpressionAst.getBinaryType()) {
            case BinaryExpressionAST::BinaryType::PLUS: return leftResult.value() + rightResult.value();
            case BinaryExpressionAST::BinaryType::MINUS: return leftResult.value() - rightResult.value();
            case BinaryExpressionAST::BinaryType::MULTIPLY: return leftResult.value() * rightResult.value();
            case BinaryExpressionAST::BinaryType::DIVIDE:{
                // runtime error will be triggered in runtime . there is no error message to be triggered
                if(rightResult.value() == 0)
                    return nullopt ;
                return leftResult.value() / rightResult.value() ;
            }
        }
    }
    return nullopt ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> OptimizationVisitor::visitOptimization(UnaryExpressionAST& unaryExpressionAst) {
    // optimize unary expression
    auto result = unaryExpressionAst.input->acceptOptimization(*this) ;

    if(result)
    // change unary expression to literal if result is constant
    {
        // optimize
        unaryExpressionAst.input = make_unique<LiteralAST>(result.value()) ;

        // evaluate
        if(unaryExpressionAst.getUnaryType() == UnaryExpressionAST::UnaryType::MINUS)
            return -result.value() ;
        return result.value() ;
    }
    return nullopt ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> OptimizationVisitor::visitOptimization(IdentifierAST& identifierAst) {
    // get value of identifier . non-constant returns nullopt
    return evaluationContext.getIdentifier(identifierAst.print_token()) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> OptimizationVisitor::visitOptimization(LiteralAST& literalAst) {
    return literalAst.value ;
}
//---------------------------------------------------------------------------
OptimizationVisitor::OptimizationVisitor()  = default ;
//---------------------------------------------------------------------------
} // namespace jitcompiler::semantic
//---------------------------------------------------------------------------
