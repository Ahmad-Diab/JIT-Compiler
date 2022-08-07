#include "pljit/ParseTree.h"
#include "pljit/ParseTreeVisitor.h"
#include <iostream>
#include <utility>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler {

size_t ParseTreeNode :: node_index_incrementer {0} ;

size_t ParseTreeNode::getNodeId() const{
    return node_index ;
}

bool ParseTreeNode::isInitialized() const {
    return isCompileError;
}
CodeReference ParseTreeNode::getReference() const {
    return codeReference ;
}
CodeManager* ParseTreeNode::getManager() const {
    return codeManager ;
}
ParseTreeNode::~ParseTreeNode()  = default ;

TerminalNode::TerminalNode(CodeManager* manager, TokenStream* tokenStream) {
    codeManager = manager ;
    this->tokenStream = tokenStream ;
}
TerminalNode::TerminalNode(CodeManager* manager ,CodeReference codeReference) {
    this->codeManager = manager ;
    this->codeReference = codeReference ;
}
std::string_view TerminalNode::print_token() const {
    size_t line = codeReference.getLineRange().first ;
    size_t begin = codeReference.getStartLineRange().first ;
    size_t last = codeReference.getStartLineRange().second ;
    return codeManager->getCurrentLine(line).substr(begin , last - begin + 1) ;
}
NonTerminalNode::NonTerminalNode(CodeManager* manager, TokenStream* tokenStream) {
    codeManager = manager ;
    this->tokenStream = tokenStream ;
}
const ParseTreeNode& NonTerminalNode::getChild(const size_t index) const {
    return *children[index].get();
}
std::size_t NonTerminalNode::num_children() const {
    return children.size() ;
}
std::unique_ptr<ParseTreeNode> NonTerminalNode::releaseChild(const size_t index) {
    unique_ptr<ParseTreeNode> node = move(children[index]);
    return node;
}

ParseTreeNode::Type FunctionDeclaration::getType() const {
    return Type::FUNCTION_DECLARATION ;
}
FunctionDeclaration::FunctionDeclaration(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager , tokenStream) {
}
bool FunctionDeclaration::recursiveDecentParser() {
    { // PARAMETER
        unique_ptr<ParameterDeclaration> parameter_ptr = make_unique<ParameterDeclaration>(codeManager, tokenStream);
        if (parameter_ptr.get()->recursiveDecentParser()) // optional
            children.emplace_back(move(parameter_ptr));
    }
    { // VARIABLE
        unique_ptr<VariableDeclaration> variable_ptr = make_unique<VariableDeclaration>(codeManager, tokenStream);
        if (variable_ptr.get()->recursiveDecentParser()) // optional
            children.emplace_back(move(variable_ptr));
    }
    { // CONSTANT
        unique_ptr<ConstantDeclaration> constant_ptr = make_unique<ConstantDeclaration>(codeManager, tokenStream);
        if (constant_ptr.get()->recursiveDecentParser()) // optional
            children.emplace_back(move(constant_ptr));
    }
    { // COMPOUND
        unique_ptr<CompoundStatement> compound_ptr = make_unique<CompoundStatement>(codeManager, tokenStream);
        if (compound_ptr.get()->recursiveDecentParser())
            children.emplace_back(move(compound_ptr));
        else {
            isCompileError = true;
            return false;
        }
    }
    { // TERMINATOR
        if(tokenStream->isEmpty())
        {
            codeManager->printCompileError(1 , ".") ;
            return false ;
        }
        else if (tokenStream->getNextToken().type == TokenStream::TokenType::TERMINATOR) {
            TokenStream::Token token = tokenStream->getNextToken();
            tokenStream->popNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager , CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
            children.emplace_back(move(genericToken));
        }
        else {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , ".") ;
            isCompileError = true;
            return false;

        }
    }
    node_index = node_index_incrementer++ ;
    return true;
}
void FunctionDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this);
}

ParseTreeNode::Type ParameterDeclaration::getType() const {
    return Type::PARAMETER_DECLARATION ;
}
ParameterDeclaration::ParameterDeclaration(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool ParameterDeclaration::recursiveDecentParser() {
    {
        if (tokenStream->getNextToken().type == TokenStream::KEYWORD)
        {
            TokenStream::Token token = tokenStream->getNextToken() ;
            string_view line = codeManager->getCurrentLine(token.line) ;
            string_view token_str = line.substr(token.start_index , token.last_index - token.start_index + 1) ;
            if(token_str == "PARAM") {

                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager , CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();

            }
            else {
                isCompileError = true ;
                return false ;
            }
        }
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        unique_ptr<DeclartorList> declartorList = make_unique<DeclartorList>(codeManager , tokenStream) ;
        if(declartorList->recursiveDecentParser())
            children.emplace_back(move(declartorList)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    {
        // inside terminal node
        if(tokenStream->getNextToken().type == TokenStream::SEMI_COLON_SEPARATOR)
        {
            TokenStream::Token token = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager , CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();
        }
        else
        {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , ";") ;
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true;
}
void ParameterDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
VariableDeclaration::VariableDeclaration(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
    node_index = node_index_incrementer++ ;
}
ParseTreeNode::Type VariableDeclaration::getType() const {
    return Type::VARIABLE_DECLARATION ;
}
bool VariableDeclaration::recursiveDecentParser() {
    {
        if (tokenStream->getNextToken().type == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream->getNextToken() ;
            string_view line = codeManager->getCurrentLine(token.line) ;
            string_view token_str = line.substr(token.start_index , token.last_index - token.start_index + 1) ;
            if(token_str == "VAR") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();
            }
            else {
                isCompileError = true ;
                return false ;
            }
        }
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        unique_ptr<DeclartorList> declartorList = make_unique<DeclartorList>(codeManager , tokenStream) ;
        if(declartorList->recursiveDecentParser())
            children.emplace_back(move(declartorList)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    {
        // inside terminal node
        if(tokenStream->getNextToken().type == TokenStream::SEMI_COLON_SEPARATOR)
        {
            TokenStream::Token token = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();
            isCompileError = false;
        }
        else
        {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , ";") ;
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void VariableDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}

ParseTreeNode::Type ConstantDeclaration::getType() const {
    return Type::CONSTANT_DECLARATION ;
}
ConstantDeclaration::ConstantDeclaration(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool ConstantDeclaration::recursiveDecentParser() {
    {
        if (tokenStream->getNextToken().type == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream->getNextToken() ;
            string_view line = codeManager->getCurrentLine(token.line) ;
            string_view token_str = line.substr(token.start_index , token.last_index - token.start_index + 1) ;
            if(token_str == "CONST") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();
            }
            else {
                isCompileError = true ;
                return false ;
            }
        }
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        unique_ptr<InitDeclartorList> initDeclartorList = make_unique<InitDeclartorList>(codeManager , tokenStream) ;
        if(initDeclartorList->recursiveDecentParser())
            children.emplace_back(move(initDeclartorList)) ;
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        // inside terminal node
        if(tokenStream->getNextToken().type == TokenStream::SEMI_COLON_SEPARATOR)
        {
            TokenStream::Token token = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();
            isCompileError = false;
        }
        else
        {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , ";") ;
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true;
}
void ConstantDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type DeclartorList::getType() const {
    return Type::DECLARATOR_LIST ;
}
DeclartorList::DeclartorList(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool DeclartorList::recursiveDecentParser() {
    {
        unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager , tokenStream) ;
        if(identifierToken->recursiveDecentParser())
            children.emplace_back(move(identifierToken)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    {
        while (!tokenStream->isEmpty() && tokenStream->getNextToken().type == TokenStream::COMMA_SEPARATOR)
        {
            TokenStream::Token commaToken = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({commaToken.line, commaToken.line}, {commaToken.start_index, commaToken.last_index}, {commaToken.start_index, commaToken.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();

            unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager , tokenStream) ;
            if(identifierToken->recursiveDecentParser())
                children.emplace_back(move(identifierToken)) ;
            else
            {
                isCompileError = true ;
                return false ;
            }
        }
    }
    node_index = node_index_incrementer++ ;
    return true;
}
void DeclartorList::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type InitDeclartorList::getType() const {
    return Type::INIT_DECLARATOR_LIST ;
}
InitDeclartorList::InitDeclartorList(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool InitDeclartorList::recursiveDecentParser() {
    {
        unique_ptr<InitDeclartor> initDeclartor = make_unique<InitDeclartor>(codeManager, tokenStream);

        if (initDeclartor->recursiveDecentParser())
            children.emplace_back(move(initDeclartor));
        else {
            isCompileError = true;
            return false;
        }
    }
    {
        while (!tokenStream->isEmpty() && tokenStream->getNextToken().type == TokenStream::COMMA_SEPARATOR) {
            TokenStream::Token commaToken = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({commaToken.line, commaToken.line}, {commaToken.start_index, commaToken.last_index}, {commaToken.start_index, commaToken.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();

            unique_ptr<InitDeclartor> initDeclartor = make_unique<InitDeclartor>(codeManager, tokenStream);
            if (initDeclartor->recursiveDecentParser())
                children.emplace_back(move(initDeclartor));
            else {
                isCompileError = true;
                return false;
            }
        }
    }
    node_index = node_index_incrementer++ ;
    return true;
}
void InitDeclartorList::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type InitDeclartor::getType() const {
    return Type::INIT_DECLARATOR ;
}
InitDeclartor::InitDeclartor(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool InitDeclartor::recursiveDecentParser() {
    {
        unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager , tokenStream) ;
        if(identifierToken->recursiveDecentParser())
            children.emplace_back(move(identifierToken)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    {
        if (tokenStream->getNextToken().type == TokenStream::CONST_ASSIGNMENT)
        {
            TokenStream::Token constAssignment = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({constAssignment.line, constAssignment.line}, {constAssignment.start_index, constAssignment.last_index}, {constAssignment.start_index, constAssignment.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();
        }
        else
        {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "=") ;
            isCompileError = true ;
            return false ;
        }
    }
    {
        unique_ptr<Literal> literal_ptr = make_unique<Literal>(codeManager , tokenStream) ;
        if(literal_ptr->recursiveDecentParser())
            children.emplace_back(move(literal_ptr)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void InitDeclartor::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type CompoundStatement::getType() const {
    return Type::COMPOUND_STATEMENT ;
}
CompoundStatement::CompoundStatement(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool CompoundStatement::recursiveDecentParser() {
    {
        if (tokenStream->getNextToken().type == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream->getNextToken() ;
            string_view line = codeManager->getCurrentLine(token.line) ;
            string_view token_str = line.substr(token.start_index , token.last_index - token.start_index + 1) ;
            if(token_str == "BEGIN") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();
            }
            else {
                codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "BEGIN keyword") ;
                isCompileError = true ;
                return false ;
            }
        }
        else {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "BEGIN keyword") ;
            isCompileError = true ;
            return false ;
        }
    }
    {
        unique_ptr<StatementList> statementList = make_unique<StatementList>(codeManager , tokenStream) ;
        if(statementList->recursiveDecentParser())
            children.emplace_back(move(statementList)) ;
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        if (tokenStream->getNextToken().type == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream->getNextToken() ;
            string_view line = codeManager->getCurrentLine(token.line) ;
            string_view token_str = line.substr(token.start_index , token.last_index - token.start_index + 1) ;
            if(token_str == "END") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();
            }
            else {
                codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "END keyword") ;
                isCompileError = true ;
                return false ;
            }
        }
        else {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "END keyword") ;
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void CompoundStatement::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type StatementList::getType() const {
    return Type::STATEMENT_LIST ;
}
StatementList::StatementList(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool StatementList::recursiveDecentParser() {
    {
        unique_ptr<Statement> statement_ptr = make_unique<Statement>(codeManager, tokenStream);
        if(statement_ptr->recursiveDecentParser())
            children.emplace_back(move(statement_ptr)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    {
        while (!tokenStream->isEmpty() && tokenStream->getNextToken().type == TokenStream::SEMI_COLON_SEPARATOR) {
            TokenStream::Token commaToken = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({commaToken.line, commaToken.line}, {commaToken.start_index, commaToken.last_index}, {commaToken.start_index, commaToken.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();

            unique_ptr<Statement> statement_ptr = make_unique<Statement>(codeManager, tokenStream);
            if(statement_ptr->recursiveDecentParser())
                children.emplace_back(move(statement_ptr)) ;
            else
            {
                isCompileError = true ;
                return false ;
            }
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void StatementList::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type Statement::getType() const {
    return Type::STATEMENT ;
}
Statement::Statement(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool Statement::recursiveDecentParser() {
    if(tokenStream->getNextToken().type == TokenStream::TokenType::KEYWORD) {
        TokenStream::Token token = tokenStream->getNextToken() ;
        string_view line = codeManager->getCurrentLine(token.line) ;
        string_view token_str = line.substr(token.start_index , token.last_index - token.start_index + 1) ;
        if(token_str == "RETURN") {
            {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();
            }
            {
                unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager , tokenStream) ;
                if(additiveExpression->recursiveDecentParser())
                    children.emplace_back(move(additiveExpression)) ;
                else
                {
                    isCompileError = true;
                    return false ;
                }
            }
        }
        else
        {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "RETURN statement") ;
            isCompileError = true;
            return false ;
        }
    }
    else {
        unique_ptr<AssignmentExpression> assignmentExpression = make_unique<AssignmentExpression>(codeManager , tokenStream) ;
        if(assignmentExpression->recursiveDecentParser())
            children.emplace_back(move(assignmentExpression)) ;
        else
        {
            isCompileError = true;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void Statement::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type AdditiveExpression::getType() const {
    return Type::ADDITIVE_EXPRESSION ;
}
AdditiveExpression::AdditiveExpression(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool AdditiveExpression::recursiveDecentParser() {
    {
        unique_ptr<MultiplicativeExpression> multiplicativeExpression = make_unique<MultiplicativeExpression>(codeManager , tokenStream) ;
        if(multiplicativeExpression->recursiveDecentParser())
            children.emplace_back(move(multiplicativeExpression)) ;
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        if(!tokenStream->isEmpty() && (tokenStream->getNextToken().type == TokenStream::TokenType::PLUS_OPERATOR || tokenStream->getNextToken().type == TokenStream::TokenType::MINUS_OPERATOR))
        {
            TokenStream::Token operatorToken = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({operatorToken.line, operatorToken.line}, {operatorToken.start_index, operatorToken.last_index}, {operatorToken.start_index, operatorToken.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();


            unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager , tokenStream) ;
            if(additiveExpression->recursiveDecentParser())
                children.emplace_back(move(additiveExpression)) ;
            else {
                isCompileError = true ;
                return false ;
            }
        }
    }
    node_index = node_index_incrementer++ ;
    return true;
}
void AdditiveExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type MultiplicativeExpression::getType() const {
    return Type::MULTIPLICATIVE_EXPRESSION ;
}
MultiplicativeExpression::MultiplicativeExpression(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool MultiplicativeExpression::recursiveDecentParser() {
    {
        unique_ptr<UnaryExpression> unaryExpression = make_unique<UnaryExpression>(codeManager, tokenStream);
        if (unaryExpression->recursiveDecentParser())
            children.emplace_back(move(unaryExpression)) ;
        else {
            isCompileError = true ;
            return false ;
        }
    }
    {
        if(!tokenStream->isEmpty() && (tokenStream->getNextToken().type == TokenStream::TokenType::MULTIPLY_OPERATOR || tokenStream->getNextToken().type == TokenStream::TokenType::DIVIDE_OPERATOR))
        {
            TokenStream::Token operatorToken = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({operatorToken.line, operatorToken.line}, {operatorToken.start_index, operatorToken.last_index}, {operatorToken.start_index, operatorToken.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();

            unique_ptr<MultiplicativeExpression> multiplicativeExpression = make_unique<MultiplicativeExpression>(codeManager , tokenStream) ;
            if(multiplicativeExpression->recursiveDecentParser())
                children.emplace_back(move(multiplicativeExpression)) ;
            else {
                isCompileError = true ;
                return false ;
            }
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void MultiplicativeExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type AssignmentExpression::getType() const {
    return Type::ASSIGNMENT_EXPRESSION ;
}
AssignmentExpression::AssignmentExpression(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool AssignmentExpression::recursiveDecentParser() {
    {
        unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager , tokenStream) ;
        if(identifierToken->recursiveDecentParser())
            children.emplace_back(move(identifierToken)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    {
        if(tokenStream->getNextToken().type == TokenStream::TokenType::VAR_ASSIGNMENT) {
            TokenStream::Token token = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();

            unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager , tokenStream) ;
            if(additiveExpression->recursiveDecentParser()) {
                children.emplace_back(move(additiveExpression)) ;
            }
            else {
                isCompileError = true ;
                return false ;
            }
        }
        else {
            codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , ":=") ;
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void AssignmentExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type UnaryExpression::getType() const {
    return Type::UNARY_EXPRESSION ;
}
UnaryExpression::UnaryExpression(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool UnaryExpression::recursiveDecentParser() {
    {
        if ((tokenStream->getNextToken().type == TokenStream::TokenType::PLUS_OPERATOR) || (tokenStream->getNextToken().type == TokenStream::TokenType::MINUS_OPERATOR)) {
            TokenStream::Token token = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({token.line, token.line}, {token.start_index, token.last_index}, {token.start_index, token.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();
        }
        unique_ptr<PrimaryExpression> primaryExpression = make_unique<PrimaryExpression>(codeManager , tokenStream) ;
        if(primaryExpression->recursiveDecentParser())
            children.emplace_back(move(primaryExpression)) ;
        else {
            isCompileError = true ;
            return false ;
        }
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void UnaryExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type PrimaryExpression::getType() const {
    return Type::PRIMARY_EXPRESSION ;
}
PrimaryExpression::PrimaryExpression(CodeManager* manager, TokenStream* tokenStream) : NonTerminalNode(manager, tokenStream) {
}
bool PrimaryExpression::recursiveDecentParser() {
    if(tokenStream->getNextToken().type == TokenStream::TokenType::IDENTIFIER)
    {
        unique_ptr<Identifier> identifier = make_unique<Identifier>(codeManager , tokenStream) ;
        if(identifier->recursiveDecentParser())
            children.emplace_back(move(identifier)) ;
        else
        {
            isCompileError = true ;
            return false ;
        }
    }
    else if(tokenStream->getNextToken().type == TokenStream::TokenType::LITERAL)
    {
        unique_ptr<Literal> literal = make_unique<Literal>(codeManager , tokenStream) ;
        if(literal->recursiveDecentParser())
            children.emplace_back(move(literal)) ;
        else {
            isCompileError = true ;
            return false ;
        }
    }
    else if(tokenStream->getNextToken().type == TokenStream::TokenType::OPEN_BRACKET)
    {
        {
            TokenStream::Token open_bracket_token = tokenStream->getNextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({open_bracket_token.line, open_bracket_token.line}, {open_bracket_token.start_index, open_bracket_token.last_index}, {open_bracket_token.start_index, open_bracket_token.last_index}));
            children.emplace_back(move(genericToken));
            tokenStream->popNextToken();
        }
        {
            unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager , tokenStream) ;
            if(additiveExpression->recursiveDecentParser())
                children.emplace_back(move(additiveExpression)) ;
            else {
                isCompileError = true ;
                return false ;
            }
        }
        {
            if(tokenStream->getNextToken().type == TokenStream::TokenType::CLOSE_BRACKET)
            {
                TokenStream::Token close_bracket_token = tokenStream->getNextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,CodeReference({close_bracket_token.line, close_bracket_token.line}, {close_bracket_token.start_index, close_bracket_token.last_index}, {close_bracket_token.start_index, close_bracket_token.last_index}));
                children.emplace_back(move(genericToken));
                tokenStream->popNextToken();
            }
            else {
                codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , ")") ;
                isCompileError = true ;
                return false ;
            }
        }
    }
    else {
        codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "IDENTIFIER , LITERAL or Open Bracket") ;
        isCompileError = true ;
        return false ;
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void PrimaryExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type Identifier::getType() const {
    return Type::IDENTIFIER;
}
Identifier::Identifier(CodeManager* manager, TokenStream* tokenStream) : TerminalNode(manager, tokenStream) {
}
bool Identifier::recursiveDecentParser() {
    if(tokenStream->getNextToken().type == TokenStream::TokenType::IDENTIFIER) {
        TokenStream::Token identifier_token = tokenStream->getNextToken();
        this->codeReference = CodeReference({identifier_token.line, identifier_token.line}, {identifier_token.start_index, identifier_token.last_index}, {identifier_token.start_index, identifier_token.last_index}) ;
        tokenStream->popNextToken();
    }
    else {
        codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "IDENTIFIER") ;
        isCompileError = false ;
        return false ;
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void Identifier::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
Literal::Literal(CodeManager* manager, TokenStream* tokenStream) : TerminalNode(manager, tokenStream) {
}
ParseTreeNode::Type Literal::getType() const {
    return Type::LITERAL ;
}
bool Literal::recursiveDecentParser() {
    if(tokenStream->getNextToken().type == TokenStream::TokenType::LITERAL) {

        TokenStream::Token literal = tokenStream->getNextToken();
        this->codeReference = CodeReference({literal.line, literal.line}, {literal.start_index, literal.last_index}, {literal.start_index, literal.last_index}) ;
        tokenStream->popNextToken();
    }
    else {
        codeManager->printCompileError(tokenStream->getNextToken().line , tokenStream->getNextToken().start_index , tokenStream->getNextToken().last_index , "LITERAL") ;
        isCompileError = false ;
        return false ;
    }
    node_index = node_index_incrementer++ ;
    return true ;
}
void Literal::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type GenericToken::getType() const {
    return Type::GENERIC_TOKEN ;
}
GenericToken::GenericToken(CodeManager* codeManager , CodeReference codeReference) : TerminalNode(codeManager , codeReference) {
    node_index = node_index_incrementer++ ;
}
bool GenericToken::recursiveDecentParser() {
    return true;
}
void GenericToken::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
} // namespace jitcompiler
//---------------------------------------------------------------------------
