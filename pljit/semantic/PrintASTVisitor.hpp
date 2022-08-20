#ifndef PLJIT_PRINTASTVISITOR_HPP
#define PLJIT_PRINTASTVISITOR_HPP

#include "ASTVisitor.hpp"
#include "AST.hpp"
#include <sstream>

//---------------------------------------------------------------------------
namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------
template<bool isLabeled>
class PrintASTVisitor ;

using TestPrintASTVisitor = PrintASTVisitor<false> ;
using VisualizeASTVisitor = PrintASTVisitor<true> ;

template<bool isLabeled>
class PrintASTVisitor final : public ASTVisitor {
    private:
    std::ostringstream buf ;
    private:
    void print_expression(const ASTNode& parentNode , const ExpressionAST& expressionAst , const std::string_view parentLabel = "") {
        switch (expressionAst.getType()) {
            case ASTNode::Type::BINARY_EXPRESSION:
            {
                const BinaryExpressionAST& curChild = static_cast<const BinaryExpressionAST&>(expressionAst)  ;
                if(isLabeled) {
                    buf << '\t' << curChild.getNodeID() << " [label=\"";
                    switch (curChild.getBinaryType()) {
                        case BinaryExpressionAST::BinaryType::PLUS: buf << "BinaryExpression(+)"; break;
                        case BinaryExpressionAST::BinaryType::MINUS: buf << "BinaryExpression(-)"; break;
                        case BinaryExpressionAST::BinaryType::MULTIPLY: buf << "BinaryExpression(*)"; break;
                        case BinaryExpressionAST::BinaryType::DIVIDE: buf << "BinaryExpression(/)"; break;
                    }
                    buf << "\"];\n";
                    buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
                }
                else {
                   buf << '\t' << parentLabel << " -> " ;
                   switch (curChild.getBinaryType()) {
                       case BinaryExpressionAST::BinaryType::PLUS: buf << "BinaryExpression(+)"; break;
                       case BinaryExpressionAST::BinaryType::MINUS: buf << "BinaryExpression(-)"; break;
                       case BinaryExpressionAST::BinaryType::MULTIPLY: buf << "BinaryExpression(*)"; break;
                       case BinaryExpressionAST::BinaryType::DIVIDE: buf << "BinaryExpression(/)"; break;
                   }
                   buf << ";\n" ;
                }
            }
            break ;
            case ASTNode::Type::UNARY_EXPRESSION:
            {
                const UnaryExpressionAST& curChild = static_cast<const UnaryExpressionAST&>(expressionAst)  ;
                if(isLabeled) {
                    buf << '\t' << curChild.getNodeID() << " [label=\"";
                    switch (curChild.getUnaryType()) {
                        case UnaryExpressionAST::UnaryType::PLUS: buf << "UnaryExpression(+)"; break;
                        case UnaryExpressionAST::UnaryType::MINUS: buf << "UnaryExpression(-)"; break;
                    }
                    buf << "\"];\n";
                    buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
                }
                else {
                    buf << '\t' << parentLabel << "-> " ;
                    switch (curChild.getUnaryType()) {
                        case UnaryExpressionAST::UnaryType::PLUS: buf << "UnaryExpression(+)"; break;
                        case UnaryExpressionAST::UnaryType::MINUS: buf << "UnaryExpression(-)"; break;
                    }
                    buf << ";\n" ;
                }
            }
            break ;
            case ASTNode::Type::IDENTIFIER :
            {
                const IdentifierAST& curChild = static_cast<const IdentifierAST&>(expressionAst)  ;
                if(isLabeled) {
                    buf << '\t' << curChild.getNodeID() << " [label=\"" << curChild.print_token() << "\"];\n";
                    buf << '\t' << parentNode.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
                }
                else {
                    // TODO Identifier -> "id"
                    buf << '\t' << parentLabel << " -> " << "Identifier;\n" ;
                }
            }
            break ;
            case ASTNode::Type::LITERAL:
            {
                const LiteralAST& curChild = static_cast<const LiteralAST&>(expressionAst)  ;
                if(isLabeled) {
                    buf << '\t' << curChild.getNodeID() << " [label=\"" << curChild.getValue() << "\"];\n";
                    buf << '\t' << parentNode.getNodeID() << " -> \"" << curChild.getNodeID() << "\";\n";
                }
                else {
                    buf << '\t' << parentLabel << " -> " << "Literal;\n" ;
                }
            }
            break ;

            default: break ;
        }
    }

    public:
    void visit(const FunctionAST& functionAst) override {
        if(isLabeled)
            buf << '\t' <<  functionAst.getNodeID() << " [label=\"" << "Function" << "\"];\n" ;

        for(size_t index = 0 ; index < functionAst.statement_size() ; ++index) {
            const StatementAST& curChild = functionAst.getStatement(index) ;
            if(curChild.getType() == ASTNode::Type::RETURN_STATEMENT) {
                if(isLabeled) {
                    buf << '\t' << curChild.getNodeID() << " [label=\""
                        << "Return Statement"
                        << "\"];\n";
                    buf << '\t' << functionAst.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
                }
                else {
                    buf << '\t' << "Function" << " -> " << "Return Statement;\n" ;
                }
            }
            else {
                if(isLabeled) {
                    buf << '\t' << curChild.getNodeID() << " [label=\""
                        << "Assignment Statement(:=)"
                        << "\"];\n";
                    buf << '\t' << functionAst.getNodeID() << " -> " << curChild.getNodeID() << ";\n";
                }
                else {
                    buf << '\t' << "Function" << " -> " << "Assignment Statement(:=);\n" ;

                }
            }
        }
        for(size_t index = 0 ; index < functionAst.statement_size() ; ++index) {
            const StatementAST& curChild = functionAst.getStatement(index) ;
            curChild.accept(*this);
        }
    }
    void visit(const ReturnStatementAST& returnStatementAst) override {
        if(isLabeled)
            print_expression(returnStatementAst, returnStatementAst.getInput());
        else
            print_expression(returnStatementAst, returnStatementAst.getInput() , "Return Statement");
        returnStatementAst.getInput().accept(*this);
    }
    void visit(const AssignmentStatementAST& assignmentStatementAst) override {
        const IdentifierAST& identifierAst = static_cast<const IdentifierAST&>(assignmentStatementAst.getLeftIdentifier()) ;
        const ExpressionAST& expressionAst = static_cast<const ExpressionAST&>(assignmentStatementAst.getRightExpression()) ;
        if(isLabeled) {
            buf << '\t' << identifierAst.getNodeID() << " [label=\"" << identifierAst.print_token() << "\"];\n";
            buf << '\t' << assignmentStatementAst.getNodeID() << " -> " << identifierAst.getNodeID() << ";\n";
        }
        else {
            buf << '\t' << "Assignment Statement(:=) -> Identifier;\n" ;
        }
        if(isLabeled)
            print_expression(assignmentStatementAst , expressionAst) ;
        else
            print_expression(assignmentStatementAst , expressionAst , "Assignment Statement(:=)") ;
        expressionAst.accept(*this) ;

    }
    void visit(const BinaryExpressionAST& binaryExpressionAst) override {
        const ExpressionAST& leftExpression = binaryExpressionAst.getLeftExpression() ;
        const ExpressionAST& rightExpression = binaryExpressionAst.getRightExpression() ;
        if(isLabeled) {
            print_expression(binaryExpressionAst, leftExpression);
            print_expression(binaryExpressionAst, rightExpression);
        }
        else {
            switch (binaryExpressionAst.getBinaryType()) {
                case BinaryExpressionAST::BinaryType::PLUS:
                {
                    print_expression(binaryExpressionAst, leftExpression , "BinaryExpression(+)");
                    print_expression(binaryExpressionAst, rightExpression , "BinaryExpression(+)");
                }
                break;
                case BinaryExpressionAST::BinaryType::MINUS:
                {
                    print_expression(binaryExpressionAst, leftExpression , "BinaryExpression(-)");
                    print_expression(binaryExpressionAst, rightExpression , "BinaryExpression(-)");

                }
                break;
                case BinaryExpressionAST::BinaryType::MULTIPLY:
                {
                    print_expression(binaryExpressionAst, leftExpression , "BinaryExpression(*)");
                    print_expression(binaryExpressionAst, rightExpression , "BinaryExpression(*)");
                }
                break;
                case BinaryExpressionAST::BinaryType::DIVIDE:
                {
                    print_expression(binaryExpressionAst, leftExpression , "BinaryExpression(/)");
                    print_expression(binaryExpressionAst, rightExpression , "BinaryExpression(/)");
                }
                break;
            }
        }
        leftExpression.accept(*this) ;
        rightExpression.accept(*this) ;

    }
    void visit(const UnaryExpressionAST& unaryExpressionAst) override
    {
        const ExpressionAST& expressionAst = unaryExpressionAst.getInput() ;
        if(isLabeled) {
            print_expression(unaryExpressionAst, expressionAst);
        }
        else {
            switch (unaryExpressionAst.getUnaryType()) {
                case UnaryExpressionAST::UnaryType::PLUS:
                {
                    print_expression(unaryExpressionAst, expressionAst , "UnaryExpression(+)");

                }
                break;
                case UnaryExpressionAST::UnaryType::MINUS:
                {
                    print_expression(unaryExpressionAst, expressionAst , "UnaryExpression(-)");
                }
                break;
            }
        }
        expressionAst.accept(*this) ;

    }
    void visit(const IdentifierAST& identifierAst) override {
        if(!isLabeled)
            buf << "\tIdentifier -> \"" << identifierAst.print_token() << "\";\n" ;
    }

    void visit(const LiteralAST& literalAst) override {
        if(!isLabeled)
            buf << "\tLiteral -> \"" << literalAst.getValue() << "\";\n" ;
    }

    void reset()  {
        buf.clear() ;
    }
    std::string getOutput() {
        return "digraph {\n" + buf.str() + "}\n" ;

    }
};

//---------------------------------------------------------------------------
} // namespace jitcompiler::semantic
//---------------------------------------------------------------------------

#endif //PLJIT_PRINTASTVISITOR_HPP
