#include "OptimizationASTVisitor.h"
#include "pljit/AST.h"
#include "pljit/EvaluationContext.h"
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

optional<int64_t> OptimizationVisitor::visitOptimization(FunctionAST& functionAst) {
    for(size_t statement_index = 0 ; statement_index < functionAst.statement_size() ; statement_index++) {
        StatementAST& statementAst = *functionAst.children[statement_index] ;
        optional<int64_t> result = statementAst.acceptOptimization(*this) ;
        if(statementAst.getType() == ASTNode::Type::RETURN_STATEMENT) {
            if(result) {
                functionAst.children.clear();
                unique_ptr<ReturnStatementAST> retStatement = make_unique<ReturnStatementAST>(make_unique<LiteralAST>(result.value())) ;
                functionAst.children.emplace_back(move(retStatement)) ;
                return result ;
            }
            else {
                functionAst.children.resize(statement_index + 1) ;
            }
        }
        else {
            if(result) {
                AssignmentStatementAST& assignmentStatementAst = static_cast<AssignmentStatementAST&>(statementAst);
                string_view identifier = assignmentStatementAst.getLeftIdentifier().print_token() ;
                assignmentStatementAst.rightExpression = make_unique<LiteralAST>(result.value()) ;
                evaluationContext.updateIdentifier(identifier , result.value()) ;
            }
        }
    }
    return nullopt;
}
std::optional<int64_t> OptimizationVisitor::visitOptimization(ReturnStatementAST& returnStatementAst) {
    return returnStatementAst.input->acceptOptimization(*this) ;
}
std::optional<int64_t> OptimizationVisitor::visitOptimization(AssignmentStatementAST& assignmentStatementAst) {
    return assignmentStatementAst.rightExpression->acceptOptimization(*this) ;
}
std::optional<int64_t> OptimizationVisitor::visitOptimization(BinaryExpressionAST& binaryExpressionAst) {
    auto leftResult = binaryExpressionAst.leftExpression->acceptOptimization(*this) ;
    auto rightResult = binaryExpressionAst.rightExpression->acceptOptimization(*this) ;

    if(leftResult)
        binaryExpressionAst.leftExpression = make_unique<LiteralAST>(leftResult.value()) ;
    if(rightResult)
        binaryExpressionAst.rightExpression = make_unique<LiteralAST>(rightResult.value()) ;
    if(leftResult && rightResult)
    {
        switch (binaryExpressionAst.getBinaryType()) {
            case BinaryExpressionAST::BinaryType::PLUS: return leftResult.value() + rightResult.value();
            case BinaryExpressionAST::BinaryType::MINUS: return leftResult.value() - rightResult.value();
            case BinaryExpressionAST::BinaryType::MULTIPLY: return leftResult.value() * rightResult.value();
            case BinaryExpressionAST::BinaryType::DIVIDE:{
                if(rightResult.value() == 0)
                    return nullopt ;
                return leftResult.value() / rightResult.value() ;
            }
        }
    }
    return nullopt ;
}
std::optional<int64_t> OptimizationVisitor::visitOptimization(UnaryExpressionAST& unaryExpressionAst) {
    auto result = unaryExpressionAst.input->acceptOptimization(*this) ;
    if(result)
    {
        unaryExpressionAst.input = make_unique<LiteralAST>(result.value()) ;
        if(unaryExpressionAst.getUnaryType() == UnaryExpressionAST::UnaryType::MINUS)
            return -result.value() ;
        return result.value() ;
    }
    return nullopt ;
}
std::optional<int64_t> OptimizationVisitor::visitOptimization(IdentifierAST& identifierAst) {
    return evaluationContext.getIdentifier(identifierAst.print_token()) ;
}
std::optional<int64_t> OptimizationVisitor::visitOptimization(LiteralAST& literalAst) {
    return literalAst.value ;
}

//---------------------------------------------------------------------------
}// namespace jitcompiler
//---------------------------------------------------------------------------
