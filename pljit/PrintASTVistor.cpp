#include "PrintASTVistor.h"
#include "AST.h"
//---------------------------------------------------------------------------
//using namespace std ;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------
namespace {
    void print_expression(std::ostream &buf , const ASTNode& parentNode , const ExpressionAST& expressionAst) {
        switch (expressionAst.getType()) {
            case ASTNode::Type::BINARY_EXPRESSION:
            {
                const BinaryExpressionAST& curChild = static_cast<const BinaryExpressionAST&>(expressionAst)  ;
                buf << '\t' << curChild.getNodeID() << " [label=\"" ;
                switch (curChild.getBinaryType()) {
                    case BinaryExpressionAST::BinaryType::PLUS :buf << '+' ; break ;
                    case BinaryExpressionAST::BinaryType::MINUS : buf << '-' ; break ;
                    case BinaryExpressionAST::BinaryType::MULTIPLY : buf << '*' ; break ;
                    case BinaryExpressionAST::BinaryType::DIVIDE : buf << '/' ; break ;
                    default: break ;
                }
                buf << "\"];\n" ;
                buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
            }
            break ;
            case ASTNode::Type::UNARY_EXPRESSION:
            {
                const UnaryExpressionAST& curChild = static_cast<const UnaryExpressionAST&>(expressionAst)  ;
                switch (curChild.getUnaryType()) {
                    case UnaryExpressionAST::UnaryType::PLUS : buf << '+' ; break ;
                    case UnaryExpressionAST::UnaryType::MINUS : buf << '-' ; break ;
                    default: break ;
                }
                buf << "\"];\n" ;
                buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
            }
            break ;
            case ASTNode::Type::IDENTIFIER :
            {
                const IdentifierAST& curChild = static_cast<const IdentifierAST&>(expressionAst)  ;
                buf << '\t' << curChild.getNodeID() << " [label=\"" << curChild.print_token() << "\"];\n";
                buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
            }
            break ;
            case ASTNode::Type::LITERAL:
            {
                const LiteralAST& curChild = static_cast<const LiteralAST&>(expressionAst)  ;
                buf << '\t' << curChild.getNodeID() << " [label=\"" << curChild.getValue() << "\"];\n";
                buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
            }
            break ;

            default: break ;
        }
    }
} // anonymous namespace

void PrintASTVistor::visit(const FunctionAST& functionAst) {
    buf << '\t' <<  functionAst.getNodeID() << " [label=\"" << "Function" << "\"];\n" ;

    for(size_t index = 0 ; index < functionAst.statement_size() ; ++index) {
        const StatementAST& curChild = functionAst.getStatement(index) ;
        if(curChild.getType() == ASTNode::Type::RETURN_STATEMENT) {
            buf << '\t' << curChild.getNodeID() << " [label=\"" << "Return Statement" << "\"];\n";
            buf << '\t' << functionAst.getNodeID() << " -> " << curChild.getNodeID() << ";\n" ;
        }
        else {
            buf << '\t' << curChild.getNodeID() << " [label=\"" << "Assignment Statement" << "\"];\n";
            buf << '\t' << functionAst.getNodeID() << " -> " << curChild.getNodeID() << ";\n" ;
        }
    }
    for(size_t index = 0 ; index < functionAst.statement_size() ; ++index) {
        const StatementAST& curChild = functionAst.getStatement(index) ;
        curChild.accept(*this);
    }
}
void PrintASTVistor::visit(const ReturnStatementAST& returnStatementAst)  {
    print_expression(buf , returnStatementAst , returnStatementAst.getInput()) ;
    returnStatementAst.getInput().accept(*this) ;
}
void PrintASTVistor::visit(const AssignmentStatementAST& assignmentStatementAst) {
    const IdentifierAST& identifierAst = static_cast<const IdentifierAST&>(assignmentStatementAst.getLeftIdentifier()) ;
    const ExpressionAST& expressionAst = static_cast<const ExpressionAST&>(assignmentStatementAst.getRightExpression()) ;
    buf << '\t' << identifierAst.getNodeID() << " [label=\"" << identifierAst.print_token() << "\"];\n";
    buf << '\t' << assignmentStatementAst.getNodeID() << " -> " << identifierAst.getNodeID() << ";\n";
    print_expression(buf , assignmentStatementAst , expressionAst) ;
    expressionAst.accept(*this) ;
}
void PrintASTVistor::visit(const BinaryExpressionAST& binaryExpressionAst) {
    const ExpressionAST& leftExpression = binaryExpressionAst.getLeftExpression() ;
    const ExpressionAST& rightExpression = binaryExpressionAst.getRightExpression() ;
    print_expression(buf , binaryExpressionAst , leftExpression) ;
    print_expression(buf , binaryExpressionAst , rightExpression) ;
    leftExpression.accept(*this) ;
    rightExpression.accept(*this) ;
}
void PrintASTVistor::visit(const UnaryExpressionAST& unaryExpressionAst) {
    const ExpressionAST& expressionAst = unaryExpressionAst.getInput() ;
    print_expression(buf , unaryExpressionAst , expressionAst) ;
    expressionAst.accept(*this) ;
}
void PrintASTVistor::visit(const IdentifierAST& /*identifierAst*/) {

}
void PrintASTVistor::visit(const LiteralAST& /*literalAst*/) {

}
void PrintASTVistor::reset() {
    buf.clear() ;
}
std::string PrintASTVistor::getOutput()  {
    return buf.str() ;
}

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
