#include "pljit/AST.h"
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler {

namespace {
    int64_t str_to_int64(string_view literal) {
        int64_t value = 0 ;
        for(char c : literal) {
            assert('0' <= c && c <= '9') ;
            value = value * 10 + static_cast<int64_t>(c - '0') ;
        }
        return value ;
    }
//    unique_ptr<StatementAST> analyzeStatement(Statement& parseTreeNode)
//    unique_ptr<ExpressionAST> analyzeExpression(AdditiveExpression& additiveExpression) ;
//    unique_ptr<ExpressionAST> analyzeExpression(MultiplicativeExpression& multiplicativeExpression) ;
//    unique_ptr<ExpressionAST> analyzeExpression(UnaryExpression& unaryExpression) ;
//    unique_ptr<ExpressionAST> analyzeExpression(PrimaryExpression& primaryExpression) ;
//    unique_ptr<ExpressionAST> analyzeExpression(Identifier& identifier) ;
//    unique_ptr<ExpressionAST> analyzeExpression(Literal& literal) ;
}
bool SymbolTable::DeclarationMap::isDeclared(std::string_view identifier) const {
    return declarationReference.find(identifier) != declarationReference.end() ;
}
bool SymbolTable::DeclarationMap::isDefined(std::string_view identifier) const {
    return declarationValue.find(identifier) != declarationValue.end() ;
}
void SymbolTable::DeclarationMap::insert(std::string_view identifier, CodeReference codeReference) {
    assert(!isDeclared(identifier));
    declarationReference[identifier]= codeReference ;
}
void SymbolTable::DeclarationMap::set_identifer(std::string_view identifier, int64_t value) {
    assert(isDeclared(identifier)) ;
    declarationValue[identifier] = value ;
}
int64_t SymbolTable::DeclarationMap::get_identifier(std::string_view identifier) {
    assert(isDefined(identifier)) ;
    return declarationValue[identifier] ;
}
bool SymbolTable::addAttributes(const ParameterDeclaration& declaration) {
    const DeclartorList& declartorList = static_cast<const DeclartorList&>(declaration.getChild(1)) ;
    for(size_t index = 0 ; declartorList.num_children() ; ++index) {
        const TerminalNode& curChild = static_cast<const TerminalNode&>(declartorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::IDENTIFIER) {
            if(!this->isDeclared(curChild.print_token())) {
                parameterDeclaration.insert(curChild.print_token(), curChild.getReference());
            }
            else {
                // TODO compile error
                isCompiled = false ;
                return false ;
            }
        }
    }
    return true ;
}

bool SymbolTable::addAttributes(const VariableDeclaration& declaration) {
    const DeclartorList& declartorList = static_cast<const DeclartorList&>(declaration.getChild(1)) ;
    for(size_t index = 0 ; declartorList.num_children() ; ++index) {
        const TerminalNode& curChild = static_cast<const TerminalNode&>(declartorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::IDENTIFIER) {
            if(!this->isDeclared(curChild.print_token()))
                variableDeclaration.insert(curChild.print_token() , curChild.getReference()) ;
            else {
                // TODO compiler error
                isCompiled = false ;
                return false ;
            }
        }
    }
    return true ;
}
bool SymbolTable::addAttributes(const ConstantDeclaration& declaration) {
    const InitDeclartorList& declartorList = static_cast<const InitDeclartorList&>(declaration.getChild(1)) ;
    for(size_t index = 0 ; declartorList.num_children() ; ++index) {
        const ParseTreeNode& curChild = static_cast<const ParseTreeNode&>(declartorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::INIT_DECLARATOR) {
            const InitDeclartor& init_declarator = static_cast<const InitDeclartor&>(curChild);
            const Identifier& identifier = static_cast<const Identifier&>(init_declarator.getChild(0)) ;
            const Literal& literal = static_cast<const Literal&>(init_declarator.getChild(2)) ;
            int64_t value = str_to_int64(literal.print_token()) ;
            if(!isDeclared(identifier.print_token())) {
                constantDeclaration.insert(identifier.print_token(), curChild.getReference());
                constantDeclaration.set_identifer(identifier.print_token(), value);
            }
            else {
                // TODO compile error
                isCompiled = false ;
                return false ;
            }
        }
    }
    return true ;
}
SymbolTable::SymbolTable(FunctionDeclaration& functionDeclaration) {
    for(size_t index = 0 ; index < functionDeclaration.num_children() ; ++index) {
        const ParseTreeNode& curNode = functionDeclaration.getChild(index) ;
        if(curNode.getType() == ParseTreeNode::Type::PARAMETER_DECLARATION)
        {
            const ParameterDeclaration& declaration = static_cast<const ParameterDeclaration&>(curNode) ;
            isCompiled &= addAttributes(declaration) ;
        }
        else if(curNode.getType() == ParseTreeNode::Type::VARIABLE_DECLARATION)
        {
            const VariableDeclaration& declaration = static_cast<const VariableDeclaration&>(curNode) ;
            isCompiled &= addAttributes(declaration) ;
        }
        else if(curNode.getType() == ParseTreeNode::Type::CONSTANT_DECLARATION)
        {
            const ConstantDeclaration& declaration = static_cast<const ConstantDeclaration&>(curNode) ;
            isCompiled &= addAttributes(declaration) ;
        }
    }
}
bool SymbolTable::isComplied() const {
    return isCompiled ;
}
//---------------------------------------------------------------------------
ASTNode::Type FunctionAST::getType() const{
    return ASTNode::Type::FUNCTION ;
}
FunctionAST::FunctionAST(CodeManager* manager) {
    this->codeManager = manager ;
    node_index = node_index_incrementer++ ;
}
ASTNode::Type AssignmentStatementAST::getType() const {
    return ASTNode::Type::ASSIGNMENT_STATEMENT;
}
ASTNode::Type ReturnStatementAST::getType() const {
    return ASTNode::Type::RETURN_STATEMENT;
}
ASTNode::Type BinaryExpressionAST::getType() const {
    return ASTNode::Type::BINARY_EXPRESSION;
}
ASTNode::Type UnaryExpressionAST::getType() const {
    return ASTNode::Type::UNARY_EXPRESSION;
}
ASTNode::Type IdentifierAST::getType() const {
    return ASTNode::Type::IDENTIFIER;
}
ASTNode::Type LiteralAST::getType() const {
    return ASTNode::Type::LITERAL;
}
size_t ASTNode::getNodeID() const{
    return node_index;
}
CodeReference ASTNode::getReference() const {
    return codeReference;
}
} //namespace jitcompiler
//---------------------------------------------------------------------------
