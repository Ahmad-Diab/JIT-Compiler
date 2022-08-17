#ifndef PLJIT_PRINTPARSETREEVISITOR_HPP
#define PLJIT_PRINTPARSETREEVISITOR_HPP
//---------------------------------------------------------------------------
#include "ParseTree.hpp"
#include "ParseTreeVisitor.hpp"
#include <sstream>
namespace jitcompiler{
//---------------------------------------------------------------------------
/*
 * isLabeld-> true(for visualization) , false (for testing)
 */
template<bool isLabeled>
class PrintVisitor ;

using TestPrintVisitor = PrintVisitor<false> ; // recommended for testing
using VisualizePrintVisitor = PrintVisitor<true> ; // recommended for visualization in DOT format (Not readable)

template<bool isLabeled>
class PrintVisitor  : public ParseTreeVisitor {
    private:
    std::ostringstream buf ;
    public:
    void visit(const FunctionDeclaration& functionDeclaration) override {
            if(isLabeled)
                buf << '\t' << functionDeclaration.getNodeId() << " [label=\"" << "function-definition" << "\"];\n";

            for (size_t indexChild = 0; indexChild < functionDeclaration.num_children(); ++indexChild) {
                const ParseTreeNode& curChild = functionDeclaration.getChild(indexChild);
                switch (curChild.getType()) {
                    case ParseTreeNode::Type::PARAMETER_DECLARATION: {
                        if(isLabeled) {
                            buf << '\t' << curChild.getNodeId() << " [label=\""<< "parameter-declaration" << "\"];\n";
                            buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                        }
                        else {
                            buf << '\t' << "function-declaration" << " -> " << "parameter-declaration" << ";\n";
                        }
                    }
                    break;
                    case ParseTreeNode::Type::VARIABLE_DECLARATION:
                    {
                        if(isLabeled) {
                            buf << '\t' << curChild.getNodeId() << " [label=\"" << "variable-declaration" << "\"];\n";
                            buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                        }
                        else {
                            buf << '\t' << "function-declaration" << " -> " << "variable-declaration" << ";\n";
                        }
                    }
                    break ;
                    case ParseTreeNode::Type::CONSTANT_DECLARATION:
                    {
                        if(isLabeled) {
                            buf << '\t' << curChild.getNodeId() << " [label=\"" << "constant-declaration" << "\"];\n";
                            buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                        }
                        else {
                            buf << '\t' << "function-declaration" << " -> " << "constant-declaration" << ";\n";

                        }
                    }
                    break;
                    case ParseTreeNode::Type::COMPOUND_STATEMENT: {
                        if(isLabeled) {
                            buf << '\t' << curChild.getNodeId() << " [label=\""
                                << "compound-declaration"
                                << "\"];\n";
                            buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                        }
                        else {
                            buf << '\t' << "function-declaration" << " -> " << "compound-declaration" << ";\n";
                        }
                    }
                    break;
                    case ParseTreeNode::Type::GENERIC_TOKEN:
                    {
                        const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                        if(isLabeled) {
                            buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                            buf << '\t' << functionDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                        }
                        else {
                            buf << '\t' << "function-declaration" << " -> " << genericToken.print_token() << ";\n";

                        }

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

    void visit(const ParameterDeclaration& parameterDeclaration) override {
        for (size_t indexChild = 0; indexChild < parameterDeclaration.num_children(); ++indexChild) {
            const ParseTreeNode& curChild = parameterDeclaration.getChild(indexChild);
            switch (curChild.getType()) {
                case ParseTreeNode::Type::DECLARATOR_LIST :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "declarator-list"
                            << "\"];\n";
                        buf << '\t' << parameterDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "parameter-declaration" << " -> " << "declarator-list" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN :
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << parameterDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "parameter-declaration" << " -> " << genericToken.print_token() << ";\n";
                    }
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
    void visit(const VariableDeclaration& variableDeclaration) override {

        for (size_t indexChild = 0; indexChild < variableDeclaration.num_children(); ++indexChild) {

            const ParseTreeNode& curChild = variableDeclaration.getChild(indexChild);
            switch (curChild.getType()) {
                case ParseTreeNode::Type::DECLARATOR_LIST :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "declarator-list"
                            << "\"];\n";
                        buf << '\t' << variableDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "variable-declaration" << " -> " << "declarator-list" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN :
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << variableDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "variable-declaration" << " -> " << genericToken.print_token() << ";\n";
                    }
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
    void visit(const ConstantDeclaration& constantDeclaration) override {

        for (size_t indexChild = 0; indexChild < constantDeclaration.num_children(); ++indexChild) {
            const ParseTreeNode& curChild = constantDeclaration.getChild(indexChild);
            switch (curChild.getType()) {
                case ParseTreeNode::Type::INIT_DECLARATOR_LIST :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "init-declarator-list"
                            << "\"];\n";
                        buf << '\t' << constantDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "constant-declaration" << " -> " << "init-declarator-list" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN :
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild);
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << constantDeclaration.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "constant-declaration" << " -> " << genericToken.print_token() << ";\n";
                    }
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
    void visit(const DeclartorList& declartorList) override {
        for (size_t indexChild = 0; indexChild < declartorList.num_children(); ++indexChild) {
            const ParseTreeNode& curChild = declartorList.getChild(indexChild);
            switch (curChild.getType()) {
                case ParseTreeNode::Type::IDENTIFIER:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "identifier"
                            << "\"];\n";
                        buf << '\t' << declartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "declarator-list" << " -> " << "identifier"<< ";\n";

                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&> (curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << declartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "declarator-list" << " -> " << genericToken.print_token()<< ";\n";
                    }
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
    void visit(const InitDeclartorList& initDeclartorList) override {
        for (size_t indexChild = 0; indexChild < initDeclartorList.num_children(); ++indexChild) {
            const ParseTreeNode& curChild = initDeclartorList.getChild(indexChild);
            switch (curChild.getType()) {
                case ParseTreeNode::Type::INIT_DECLARATOR:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "init-declarator"
                            << "\"];\n";
                        buf << '\t' << initDeclartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "init-declarator-list" << " -> " << "init-declarator" << ";\n";

                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&> (curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << initDeclartorList.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "init-declarator-list" << " -> " <<  genericToken.print_token()  << ";\n";
                    }
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
    void visit(const InitDeclartor& initDeclartor) override  {
        for(size_t indexChild = 0 ; indexChild < initDeclartor.num_children() ; ++indexChild) {
            const ParseTreeNode& curChild = initDeclartor.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::IDENTIFIER :{
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "identifier"
                            << "\"];\n";
                        buf << '\t' << initDeclartor.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }else {
                        buf << '\t' << "init-declarator" << " -> " << "identifier" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << initDeclartor.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "init-declarator" << " -> " << genericToken.print_token() << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::LITERAL:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << " [label=\""
                            << "literal"
                            << "\"];\n";
                        buf << '\t' << initDeclartor.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "init-declarator" << " -> " << "literal" << ";\n";
                    }
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
    void visit(const CompoundStatement& compoundStatement) override {
        for(size_t indexChild = 0 ; indexChild < compoundStatement.num_children() ; ++indexChild) {
            const ParseTreeNode& curChild = compoundStatement.getChild(indexChild) ;

            switch (curChild.getType()) {
                case ParseTreeNode::Type::STATEMENT_LIST:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "statement-list"
                            << "\"];\n";
                        buf << '\t' << compoundStatement.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "compound-statement" << " -> " << "statement-list" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << compoundStatement.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "compound-statement" << " -> " << genericToken.print_token() << ";\n";
                    }
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
    void visit(const StatementList& statementList) override {
        for(size_t indexChild = 0 ; indexChild < statementList.num_children() ; ++indexChild)
        {
            const ParseTreeNode& curChild = statementList.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::STATEMENT:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "statement"
                            << "\"];\n";
                        buf << '\t' << statementList.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "statement-list"  << " -> " << "statement" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << statementList.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "statement-list"  << " -> " << genericToken.print_token() << ";\n";
                    }
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
    void visit(const Statement& statement) override {
        for(size_t indexChild = 0 ; indexChild < statement.num_children() ; ++indexChild)
        {
            const ParseTreeNode& curChild = statement.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::ASSIGNMENT_EXPRESSION:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "assignment-expression"
                            << "\"];\n";
                        buf << '\t' << statement.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "statement"  << " -> " << "assignment-expression" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << statement.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "statement"  << " -> " << genericToken.print_token() << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "additive-expression"
                            << "\"];\n";
                        buf << '\t' << statement.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "statement"  << " -> " << "additive-expression" << ";\n";
                    }
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
    void visit(const AssignmentExpression& assignmentExpression) override {
        for(size_t indexChild = 0 ; indexChild < assignmentExpression.num_children() ; ++indexChild)
        {
            const ParseTreeNode& curChild = assignmentExpression.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::IDENTIFIER :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "identifier"
                            << "\"];\n";
                        buf << '\t' << assignmentExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "assignment-expression"  << " -> " << "identifier" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << assignmentExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "assignment-expression"  << " -> " << genericToken.print_token() << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "additive-expression"
                            << "\"];\n";
                        buf << '\t' << assignmentExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "assignment-expression"  << " -> " <<  "additive-expression" << ";\n";

                    }
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
    void visit(const AdditiveExpression& additiveExpression) override {
        for(size_t indexChild = 0 ; indexChild < additiveExpression.num_children() ; ++indexChild) {
            const ParseTreeNode& curChild = additiveExpression.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::MULTIPLICATIVE_EXPRESSION :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "multiplicative-expression"
                            << "\"];\n";
                        buf << '\t' << additiveExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "additive-expression"   << " -> " << "multiplicative-expression" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << additiveExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "additive-expression"   << " -> " << genericToken.print_token() << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::ADDITIVE_EXPRESSION: {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "additive-expression"
                            << "\"];\n";
                        buf << '\t' << additiveExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "additive-expression"   << " -> " << "additive-expression" << ";\n";
                    }
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
    void visit(const MultiplicativeExpression& multiplicativeExpression) override {
        for(size_t indexChild = 0 ; indexChild < multiplicativeExpression.num_children() ; ++indexChild) {
            const ParseTreeNode& curChild = multiplicativeExpression.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::MULTIPLICATIVE_EXPRESSION :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "multiplicative-expression"
                            << "\"];\n";
                        buf << '\t' << multiplicativeExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "multiplicative-expression"   << " -> " << "multiplicative-expression" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << multiplicativeExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "multiplicative-expression"   << " -> " << genericToken.print_token() << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::UNARY_EXPRESSION:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "unary-expression"
                            << "\"];\n";
                        buf << '\t' << multiplicativeExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "multiplicative-expression"   << " -> " << "unary-expression" << ";\n";

                    }
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
    void visit(const UnaryExpression& unaryExpression) override {
        for(size_t indexChild = 0 ; indexChild < unaryExpression.num_children() ; ++indexChild)
        {
            const ParseTreeNode& curChild = unaryExpression.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << unaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "unary-expression" << " -> " << genericToken.print_token() << ";\n";

                    }
                }
                break ;
                case ParseTreeNode::Type::PRIMARY_EXPRESSION:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "primary-expression"
                            << "\"];\n";
                        buf << '\t' << unaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "unary-expression" << " -> " << "primary-expression" << ";\n";
                    }
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
    void visit(const PrimaryExpression& primaryExpression) override {
        for(size_t indexChild = 0 ; indexChild < primaryExpression.num_children() ; ++indexChild)
        {
            const ParseTreeNode& curChild = primaryExpression.getChild(indexChild) ;
            switch (curChild.getType()) {
                case ParseTreeNode::Type::IDENTIFIER :
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "identifier"
                            << "\"];\n";
                        buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "primary-expression" << " -> " << "identifier" << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::GENERIC_TOKEN:
                {
                    const GenericToken& genericToken = static_cast<const GenericToken&>(curChild) ;
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\"" << genericToken.print_token() << "\"];\n";
                        buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "primary-expression" << " -> " << genericToken.print_token() << ";\n";
                    }
                }
                break ;
                case ParseTreeNode::Type::LITERAL:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "literal"
                            << "\"];\n";
                        buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "primary-expression" << " -> " << "literal" << ";\n";

                    }
                }
                break ;
                case ParseTreeNode::Type::ADDITIVE_EXPRESSION:
                {
                    if(isLabeled) {
                        buf << '\t' << curChild.getNodeId() << "[label=\""
                            << "additive-expression"
                            << "\"];\n";
                        buf << '\t' << primaryExpression.getNodeId() << " -> " << curChild.getNodeId() << ";\n";
                    }
                    else {
                        buf << '\t' << "primary-expression" << " -> " << "additive-expression" << ";\n";
                    }
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
    void visit(const Identifier& identifier) override {
        if(!isLabeled)
            buf << '\t' << "identifier" << " -> \"" << identifier.print_token() << "\";\n";
    }
    void visit(const Literal& literal) override {
        if(!isLabeled)
            buf << '\t' << "literal" << " -> \"" << literal.print_token() << "\";\n";
    }
    void visit(const GenericToken& /*genericToken*/) override {}

    void reset() {
        buf.clear()  ;
    }
    std::string getOutput() {
        return "digraph {\n" + buf.str() + "}\n" ;
    }
};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_PRINTPARSETREEVISITOR_HPP
