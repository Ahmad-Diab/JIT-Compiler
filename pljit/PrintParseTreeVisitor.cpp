#include "PrintParseTreeVisitor.hpp"
#include "ParseTree.hpp"
//---------------------------------------------------------------------------
//using namespace std ;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace jitcompiler{
//---------------------------------------------------------------------------
void PrintVisitor::visit(const FunctionDeclaration& functionDeclaration) {
    buf << '\t' << functionDeclaration.getNodeId() << " [label=\"" << "function-definition" << "\"];\n";

    for (size_t indexChild = 0; indexChild < functionDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = functionDeclaration.getChild(indexChild);
        switch (curChild.getType()) {
            case ParseTreeNode::Type::PARAMETER_DECLARATION: {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "parameter-declaration" << "\"];\n";
                buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
            }
            break;
            case ParseTreeNode::Type::VARIABLE_DECLARATION:
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "variable-declaration" << "\"];\n";
                buf << '\t' << functionDeclaration.getNodeId() << " -> " <<  curChild.getNodeId() <<";\n";
            }
            break ;
            case ParseTreeNode::Type::CONSTANT_DECLARATION:
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "constant-declaration" << "\"];\n";
                buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
            }
            break;
            case ParseTreeNode::Type::COMPOUND_STATEMENT: {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "compound-declaration" << "\"];\n";
                buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
            }
            break;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
            }
            break ;
            default: break;
        }
    }
    for (size_t indexChild = 0; indexChild < functionDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = functionDeclaration.getChild(indexChild);
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const ParameterDeclaration& parameterDeclaration) {
    for (size_t indexChild = 0; indexChild < parameterDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = parameterDeclaration.getChild(indexChild);
        switch (curChild.getType()) {
            case ParseTreeNode::Type::DECLARATOR_LIST :
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "declarator-list" << "\"];\n";
                buf << '\t' << parameterDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN :
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                buf << '\t' << parameterDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
            }
            break ;
            default: break ;
        }
    }

    for (size_t indexChild = 0; indexChild < parameterDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = parameterDeclaration.getChild(indexChild);
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const VariableDeclaration& variableDeclaration) {

    for (size_t indexChild = 0; indexChild < variableDeclaration.num_children(); ++indexChild) {

        const ParseTreeNode& curChild = variableDeclaration.getChild(indexChild);
        switch (curChild.getType()) {
            case ParseTreeNode::Type::DECLARATOR_LIST :
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "declarator-list" << "\"];\n";
                buf << '\t' << variableDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN :
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                buf << '\t' << variableDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }

    for (size_t indexChild = 0; indexChild < variableDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = variableDeclaration.getChild(indexChild);
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const ConstantDeclaration& constantDeclaration) {

    for (size_t indexChild = 0; indexChild < constantDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = constantDeclaration.getChild(indexChild);
        switch (curChild.getType()) {
            case ParseTreeNode::Type::INIT_DECLARATOR_LIST :
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "init-declarator-list" << "\"];\n";
                buf << '\t' << constantDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN :
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                buf << '\t' << constantDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }

    for (size_t indexChild = 0; indexChild < constantDeclaration.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = constantDeclaration.getChild(indexChild);
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const DeclartorList& declartorList) {
    for (size_t indexChild = 0; indexChild < declartorList.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = declartorList.getChild(indexChild);
        switch (curChild.getType()) {
            case ParseTreeNode::Type::IDENTIFIER:
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "identifier" << "\"];\n";
                buf << '\t' << declartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&> (curChild) ;
                buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << declartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t indexChild = 0 ; indexChild < declartorList.num_children() ; ++ indexChild)
    {
        const ParseTreeNode& curChild = declartorList.getChild(indexChild) ;
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const InitDeclartorList& initDeclartorList) {
    for (size_t indexChild = 0; indexChild < initDeclartorList.num_children(); ++indexChild) {
        const ParseTreeNode& curChild = initDeclartorList.getChild(indexChild);
        switch (curChild.getType()) {
            case ParseTreeNode::Type::INIT_DECLARATOR:
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "identifier" << "\"];\n";
                buf << '\t' << initDeclartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&> (curChild) ;
                buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << initDeclartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t indexChild = 0 ; indexChild < initDeclartorList.num_children() ; ++ indexChild)
    {
        const ParseTreeNode& curChild = initDeclartorList.getChild(indexChild) ;
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const InitDeclartor& initDeclartor) {
    for(size_t indexChild = 0 ; indexChild < initDeclartor.num_children() ; ++indexChild) {
        const ParseTreeNode& curChild = initDeclartor.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::IDENTIFIER :{
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "identifier" << "\"];\n";
                buf << '\t' << initDeclartor.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << initDeclartor.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::LITERAL:
            {
                buf << '\t' << curChild.getNodeId() << " [label=\"" << "literal" << "\"];\n";
                buf << '\t' << initDeclartor.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < initDeclartor.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = initDeclartor.getChild(index) ;
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const CompoundStatement& compoundStatement) {
    for(size_t indexChild = 0 ; indexChild < compoundStatement.num_children() ; ++indexChild) {
        const ParseTreeNode& curChild = compoundStatement.getChild(indexChild) ;

        switch (curChild.getType()) {
            case ParseTreeNode::Type::STATEMENT_LIST:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "statement-list" << "\"];\n" ;
                buf << '\t' << compoundStatement.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << compoundStatement.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < compoundStatement.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = compoundStatement.getChild(index) ;
        curChild.accept(*this) ;
    }

}
void PrintVisitor::visit(const StatementList& statementList) {
    for(size_t indexChild = 0 ; indexChild < statementList.num_children() ; ++indexChild)
    {
        const ParseTreeNode& curChild = statementList.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::STATEMENT:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "statement" << "\"];\n" ;
                buf << '\t' << statementList.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << statementList.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < statementList.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = statementList.getChild(index) ;
        curChild.accept(*this) ;
    }

}
void PrintVisitor::visit(const Statement& statement) {
    for(size_t indexChild = 0 ; indexChild < statement.num_children() ; ++indexChild)
    {
        const ParseTreeNode& curChild = statement.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::ASSIGNMENT_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "assignment-expression" << "\"];\n" ;
                buf << '\t' << statement.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << statement.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "additive-expression" << "\"];\n" ;
                buf << '\t' << statement.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < statement.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = statement.getChild(index) ;
        curChild.accept(*this) ;
    }

}
void PrintVisitor::visit(const AssignmentExpression& assignmentExpression) {
    for(size_t indexChild = 0 ; indexChild < assignmentExpression.num_children() ; ++indexChild)
    {
        const ParseTreeNode& curChild = assignmentExpression.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::IDENTIFIER :
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "identifier" << "\"];\n" ;
                buf << '\t' << assignmentExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << assignmentExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "additive-expression" << "\"];\n" ;
                buf << '\t' << assignmentExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < assignmentExpression.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = assignmentExpression.getChild(index) ;
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const AdditiveExpression& additiveExpression) {
    for(size_t indexChild = 0 ; indexChild < additiveExpression.num_children() ; ++indexChild) {
        const ParseTreeNode& curChild = additiveExpression.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::MULTIPLICATIVE_EXPRESSION :
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "multiplicative-expression" << "\"];\n" ;
                buf << '\t' << additiveExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << additiveExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "additive-expression" << "\"];\n" ;
                buf << '\t' << additiveExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < additiveExpression.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = additiveExpression.getChild(index) ;
        curChild.accept(*this) ;
    }

}
void PrintVisitor::visit(const MultiplicativeExpression& multiplicativeExpression) {
    for(size_t indexChild = 0 ; indexChild < multiplicativeExpression.num_children() ; ++indexChild) {
        const ParseTreeNode& curChild = multiplicativeExpression.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::MULTIPLICATIVE_EXPRESSION :
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "multiplicative-expression" << "\"];\n" ;
                buf << '\t' << multiplicativeExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << multiplicativeExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::UNARY_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "unary-expression" << "\"];\n" ;
                buf << '\t' << multiplicativeExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < multiplicativeExpression.num_children() ; ++index) {
        const ParseTreeNode& curChild = multiplicativeExpression.getChild(index) ;
        curChild.accept(*this) ;
    }
}
void PrintVisitor::visit(const UnaryExpression& unaryExpression) {
    for(size_t indexChild = 0 ; indexChild < unaryExpression.num_children() ; ++indexChild)
    {
        const ParseTreeNode& curChild = unaryExpression.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << unaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::PRIMARY_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "primary-expression" << "\"];\n" ;
                buf << '\t' << unaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < unaryExpression.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = unaryExpression.getChild(index) ;
        curChild.accept(*this) ;
    }

}
void PrintVisitor::visit(const PrimaryExpression& primaryExpression) {
    for(size_t indexChild = 0 ; indexChild < primaryExpression.num_children() ; ++indexChild)
    {
        const ParseTreeNode& curChild = primaryExpression.getChild(indexChild) ;
        switch (curChild.getType()) {
            case ParseTreeNode::Type::IDENTIFIER :
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "identifier" << "\"];\n" ;
                buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::GENERIC_TOKEN:
            {
                const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n" ;
                buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::LITERAL:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "literal" << "\"];\n" ;
                buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
            {
                buf << '\t' << curChild.getNodeId() << "[label=\"" << "additive-expression" << "\"];\n" ;
                buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n" ;
            }
            break ;
            default: break ;
        }
    }
    for(size_t index = 0 ; index < primaryExpression.num_children() ; ++index)
    {
        const ParseTreeNode& curChild = primaryExpression.getChild(index) ;
        curChild.accept(*this) ;
    }

}
void PrintVisitor::visit(const Identifier& /*identifier*/) {

}
void PrintVisitor::visit(const Literal& /*literal*/) {
}
void PrintVisitor::visit(const GenericToken& /*genericToken*/) {
}
std::string PrintVisitor::getOutput()  {
    return buf.str() ;
}
void PrintVisitor::reset() {
    buf.clear()  ;
}

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
