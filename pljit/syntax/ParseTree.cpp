#include "pljit/syntax/ParseTree.hpp"
#include "pljit/syntax/ParseTreeVisitor.hpp"
#include "pljit/syntax/PrintParseTreeVisitor.hpp"

#include <iostream>
#include <utility>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler ::syntax{

size_t ParseTreeNode :: node_index_incrementer {0} ;

size_t ParseTreeNode::getNodeId() const{
    return node_index ;
}
// Organization
management::CodeReference TerminalNode::getReference() const {
    return codeReference ;
}
management::CodeManager* ParseTreeNode::getManager() const {
    return codeManager ;
}
std::string ParseTreeNode::visualizeDot() const {
    VisualizePrintVisitor printVisitor ;
    this->accept(printVisitor) ;
    return printVisitor.getOutput() ;
}
std::string ParseTreeNode::testDot() const {
    TestPrintVisitor printVisitor ;
    this->accept(printVisitor) ;
    return printVisitor.getOutput() ;
}

bool ParseTreeNode::compileCode(TokenStream& tokenStream) {
    bool triggerCompilation = this->recursiveDescentParser(tokenStream) ;
    if(triggerCompilation && !tokenStream.isEmpty()) {
        /// if token stream is not empty then there is an unexpected token
        codeManager->printTokenFailure(tokenStream.lookup().getCodeReference());
        return false ;
    }
    return triggerCompilation ;
}

ParseTreeNode::~ParseTreeNode()  = default ;

TerminalNode::TerminalNode(management::CodeManager* manager) {
    node_index = node_index_incrementer++ ;
    codeManager = manager ;
}
TerminalNode::TerminalNode(management::CodeManager* manager ,management::CodeReference codeReference): TerminalNode(manager) {
    this->codeManager = manager ;
    this->codeReference = move(codeReference);
}
std::string_view TerminalNode::print_token() const {
    size_t line = codeReference.getStartLineRange().first ;
    size_t begin = codeReference.getStartLineRange().second ;
    size_t last = codeReference.getEndLineRange().second ;
    return codeManager->getCurrentLine(line).substr(begin , last - begin + 1) ;
}
NonTerminalNode::NonTerminalNode(management::CodeManager* manager) {
    node_index = node_index_incrementer++ ;
    codeManager = manager ;
}
const ParseTreeNode& NonTerminalNode::getChild(size_t index) const {
    return *children[index].get();
}
std::size_t NonTerminalNode::num_children() const {
    return children.size() ;
}

ParseTreeNode::Type FunctionDeclaration::getType() const {
    return Type::FUNCTION_DECLARATION ;
}
FunctionDeclaration::FunctionDeclaration(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool FunctionDeclaration::recursiveDescentParser(TokenStream& tokenStream) {
    { // PARAMETER
        unique_ptr<ParameterDeclaration> parameter_ptr = make_unique<ParameterDeclaration>(codeManager);
        if (parameter_ptr->recursiveDescentParser(tokenStream)) // optional
            children.emplace_back(move(parameter_ptr));
    }
    { // VARIABLE
        unique_ptr<VariableDeclaration> variable_ptr = make_unique<VariableDeclaration>(codeManager);
        if (variable_ptr->recursiveDescentParser(tokenStream)) // optional
            children.emplace_back(move(variable_ptr));
    }
    { // CONSTANT
        unique_ptr<ConstantDeclaration> constant_ptr = make_unique<ConstantDeclaration>(codeManager);
        if (constant_ptr->recursiveDescentParser(tokenStream)) // optional
            children.emplace_back(move(constant_ptr));
    }
    { // COMPOUND
        unique_ptr<CompoundStatement> compound_ptr = make_unique<CompoundStatement>(codeManager);
        if (compound_ptr->recursiveDescentParser(tokenStream))
            children.emplace_back(move(compound_ptr));
        else {
            return false;
        }
    }
    { // TERMINATOR
        if(tokenStream.isEmpty()) {
            codeManager->printCompileError(1 , ".") ;
            return false ;
        }
        else if (tokenStream.lookup().getTokenType() == TokenStream::TokenType::TERMINATOR) {
            TokenStream::Token token = tokenStream.lookup();
            tokenStream.nextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager , token.getCodeReference());
            children.emplace_back(move(genericToken));
        }
        else {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference()  , ".") ;
            return false;
        }
    }
    return true;
}
void FunctionDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this);
}
ParseTreeNode::Type ParameterDeclaration::getType() const {
    return Type::PARAMETER_DECLARATION ;
}
ParameterDeclaration::ParameterDeclaration(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool ParameterDeclaration::recursiveDescentParser(TokenStream& tokenStream) {
    { // KEYWORD "PARAM"
        if(tokenStream.isEmpty()) {
            /// no compilation error because PARAM is optional
            return false ;
        }
        if(tokenStream.lookup().getTokenType() == TokenStream::KEYWORD)
        {
            TokenStream::Token token = tokenStream.lookup() ;
            string_view line = codeManager->getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
            size_t start_index = token.getCodeReference().getStartLineRange().second ;
            size_t last_index = token.getCodeReference().getEndLineRange().second ;
            string_view token_str = line.substr(start_index , last_index - start_index + 1) ;
            if(token_str == "PARAM") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager , token.getCodeReference());
                children.emplace_back(move(genericToken));
                tokenStream.nextToken();
            }
            else {
                /// no compilation error because PARAM is optional
                return false ;
            }
        }
        else {
            /// no compilation error because PARAM is optional
            return false ;
        }
    }
    { // declarator-list
        unique_ptr<DeclaratorList> declaratorList = make_unique<DeclaratorList>(codeManager ) ;
        if(declaratorList->recursiveDescentParser(tokenStream))
            children.emplace_back(move(declaratorList)) ;
        else {
            return false ;
        }
    }
    { // semi-colon line separator
        if(tokenStream.isEmpty()) {
            // compile error , since PARAM keyword is passed
            codeManager->printCompileError(1 , ";") ;
            return false ;
        }
        // inside terminal node
        if(tokenStream.lookup().getTokenType() == TokenStream::SEMI_COLON_SEPARATOR) {
            TokenStream::Token token = tokenStream.nextToken();
            children.emplace_back(make_unique<GenericToken>(this->codeManager , token.getCodeReference()));
        }
        else {
            // compile error , since PARAM keyword is passed
            codeManager->printCompileError(tokenStream.lookup().getCodeReference() , ";") ;
            return false ;
        }
    }
    return true;
}
void ParameterDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
VariableDeclaration::VariableDeclaration(management::CodeManager* manager) : NonTerminalNode(manager) {
    node_index = node_index_incrementer++ ;
}
ParseTreeNode::Type VariableDeclaration::getType() const {
    return Type::VARIABLE_DECLARATION ;
}
bool VariableDeclaration::recursiveDescentParser(TokenStream& tokenStream) {
    { // KEYWORD "VAR"
        if(tokenStream.isEmpty()) {
            /// no compilation error because VAR is optional
            return false ;
        }
        if (tokenStream.lookup().getTokenType() == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream.lookup() ;
            string_view line = codeManager->getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
            size_t start_index = token.getCodeReference().getStartLineRange().second ;
            size_t last_index = token.getCodeReference().getEndLineRange().second ;
            string_view token_str = line.substr(start_index , last_index - start_index + 1) ;
            if(token_str == "VAR") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,token.getCodeReference());
                children.emplace_back(move(genericToken));
                tokenStream.nextToken();
            }
            else {
                return false ;
            }
        }
        else {
            return false ;
        }
    }
    { // declarator list
        unique_ptr<DeclaratorList> declaratorList = make_unique<DeclaratorList>(codeManager ) ;
        if(declaratorList->recursiveDescentParser(tokenStream))
            children.emplace_back(move(declaratorList)) ;
        else
            return false ;
    }
    { // semi-colon
        if(tokenStream.isEmpty()){
            // compile error , since VAR keyword is passed
            codeManager->printCompileError(1 , ";")  ;
            return false ;
        }
        // inside terminal node
        if(tokenStream.lookup().getTokenType() == TokenStream::SEMI_COLON_SEPARATOR) {
            TokenStream::Token token = tokenStream.nextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,token.getCodeReference());
            children.emplace_back(move(genericToken));
        }
        else
        {
            // compile error , since VAR keyword is passed
            codeManager->printCompileError(tokenStream.lookup().getCodeReference() , ";") ;
            return false ;
        }
    }
    return true ;
}
void VariableDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}

ParseTreeNode::Type ConstantDeclaration::getType() const {
    return Type::CONSTANT_DECLARATION ;
}
ConstantDeclaration::ConstantDeclaration(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool ConstantDeclaration::recursiveDescentParser(TokenStream& tokenStream) {
    { // Keyword "CONST"
        if(tokenStream.isEmpty()) {
            /// no compilation error because CONST is optional
            return false ;
        }
        if (tokenStream.lookup().getTokenType() == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream.lookup() ;
            string_view line = codeManager->getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
            size_t start_index = token.getCodeReference().getStartLineRange().second ;
            size_t last_index = token.getCodeReference().getEndLineRange().second ;
            string_view token_str = line.substr(start_index , last_index - start_index + 1) ;
            if(token_str == "CONST") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager , token.getCodeReference());
                children.emplace_back(move(genericToken));
                tokenStream.nextToken();
            }
            else {
                /// no compilation error because VAR is optional
                return false ;
            }
        }
        else {
            /// no compilation error because VAR is optional
            return false ;
        }
    }
    { // init-declarator-list
        unique_ptr<InitDeclaratorList> initDeclaratorList = make_unique<InitDeclaratorList>(codeManager) ;
        if(initDeclaratorList->recursiveDescentParser(tokenStream))
            children.emplace_back(move(initDeclaratorList)) ;
        else
            return false ;

    }
    { // semi-colon
        if(tokenStream.isEmpty())
        {
            // compile error , since CONST keyword is passed
            codeManager->printCompileError(1 , ";") ;
            return false ;
        }
        if(tokenStream.lookup().getTokenType() == TokenStream::SEMI_COLON_SEPARATOR)
        {
            TokenStream::Token token = tokenStream.nextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,token.getCodeReference());
            children.emplace_back(move(genericToken));
        }
        else
        {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference(), ";") ;
            return false ;
        }
    }
    return true;
}
void ConstantDeclaration::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type DeclaratorList::getType() const {
    return Type::DECLARATOR_LIST ;
}
DeclaratorList::DeclaratorList(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool DeclaratorList::recursiveDescentParser(TokenStream& tokenStream) {
    { // identifier
        unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager) ;
        if(identifierToken->recursiveDescentParser(tokenStream))
            children.emplace_back(move(identifierToken)) ;
        else
            return false ;
    }
    {  // {"," identifier}
        while (!tokenStream.isEmpty() && tokenStream.lookup().getTokenType() == TokenStream::COMMA_SEPARATOR)
        {
            { // ","
                TokenStream::Token commaToken = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, commaToken.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            { // "identifier"
                unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager);
                if (identifierToken->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(identifierToken));
                else {
                    return false;
                }
            }
        }
    }
    return true;
}
void DeclaratorList::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type InitDeclaratorList::getType() const {
    return Type::INIT_DECLARATOR_LIST ;
}
InitDeclaratorList::InitDeclaratorList(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool InitDeclaratorList::recursiveDescentParser(TokenStream& tokenStream) {
    { // init-declarator
        unique_ptr<InitDeclarator> initDeclarator = make_unique<InitDeclarator>(codeManager);

        if (initDeclarator->recursiveDescentParser(tokenStream))
            children.emplace_back(move(initDeclarator));
        else {
            return false;
        }
    }
    { // { "," init-declarator}
        while (!tokenStream.isEmpty() && tokenStream.lookup().getTokenType() == TokenStream::COMMA_SEPARATOR) {
            { // ","
                TokenStream::Token commaToken = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, commaToken.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            { // "init-declarator"
                unique_ptr<InitDeclarator> initDeclarator = make_unique<InitDeclarator>(codeManager);
                if (initDeclarator->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(initDeclarator));
                else {
                    return false;
                }
            }
        }
    }
    return true;
}

void InitDeclaratorList::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type InitDeclarator::getType() const {
    return Type::INIT_DECLARATOR ;
}
InitDeclarator::InitDeclarator(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool InitDeclarator::recursiveDescentParser(TokenStream& tokenStream) {
    { // identifier
        unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager ) ;
        if(identifierToken->recursiveDescentParser(tokenStream))
            children.emplace_back(move(identifierToken)) ;
        else
        {
            return false ;
        }
    }
    { // "="
        if(tokenStream.isEmpty()) {
            codeManager->printCompileError(1 , "=") ;
            return false ;
        }
        else if (tokenStream.lookup().getTokenType() == TokenStream::CONST_ASSIGNMENT) {
            TokenStream::Token constAssignment = tokenStream.nextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,constAssignment.getCodeReference());
            children.emplace_back(move(genericToken));
        }
        else {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "=") ;
            return false ;
        }
    }
    { // literal
        unique_ptr<Literal> literal_ptr = make_unique<Literal>(codeManager) ;
        if(literal_ptr->recursiveDescentParser(tokenStream))
            children.emplace_back(move(literal_ptr)) ;
        else {
            return false ;
        }
    }
    return true ;
}
void InitDeclarator::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type CompoundStatement::getType() const {
    return Type::COMPOUND_STATEMENT ;
}
CompoundStatement::CompoundStatement(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool CompoundStatement::recursiveDescentParser(TokenStream& tokenStream) {
    { // Keyword "BEGIN"
        if(tokenStream.isEmpty()) {
            codeManager->printCompileError(5 , "BEGIN") ;
            return false ;
        }
        if (tokenStream.lookup().getTokenType() == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream.lookup() ;
            string_view line = codeManager->getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
            size_t start_index = token.getCodeReference().getStartLineRange().second ;
            size_t last_index = token.getCodeReference().getEndLineRange().second ;
            string_view token_str = line.substr(start_index , last_index - start_index + 1) ;
            if(token_str == "BEGIN") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,token.getCodeReference());
                children.emplace_back(move(genericToken));
                tokenStream.nextToken();
            }
            else {
                codeManager->printCompileError(tokenStream.lookup().getCodeReference(), "BEGIN") ;
                return false ;
            }
        }
        else {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "BEGIN") ;
            return false ;
        }
    }
    { // statement-list
        unique_ptr<StatementList> statementList = make_unique<StatementList>(codeManager) ;
        if(statementList->recursiveDescentParser(tokenStream))
            children.emplace_back(move(statementList)) ;
        else {
            return false ;
        }
    }
    { // Keyword "END"
        if(tokenStream.isEmpty()) {
            codeManager->printCompileError(3 , "END") ;
            return false ;
        }
        if (tokenStream.lookup().getTokenType() == TokenStream::KEYWORD) {
            TokenStream::Token token = tokenStream.lookup() ;
            string_view line = codeManager->getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
            size_t start_index = token.getCodeReference().getStartLineRange().second ;
            size_t last_index = token.getCodeReference().getEndLineRange().second ;
            string_view token_str = line.substr(start_index , last_index - start_index + 1) ;
            if(token_str == "END") {
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,token.getCodeReference());
                children.emplace_back(move(genericToken));
                tokenStream.nextToken();
            }
            else {
                codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "END") ;
                return false ;
            }
        }
        else {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "END") ;
            return false ;
        }
    }
    return true ;
}
void CompoundStatement::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type StatementList::getType() const {
    return Type::STATEMENT_LIST ;
}
StatementList::StatementList(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool StatementList::recursiveDescentParser(TokenStream& tokenStream) {
    { // statement
        unique_ptr<Statement> statement_ptr = make_unique<Statement>(codeManager);
        if(statement_ptr->recursiveDescentParser(tokenStream))
            children.emplace_back(move(statement_ptr)) ;
        else
            return false ;
    }
    { // {";" "statement"}

        while (!tokenStream.isEmpty() && tokenStream.lookup().getTokenType() == TokenStream::SEMI_COLON_SEPARATOR) {
            { // ";"
                TokenStream::Token commaToken = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, commaToken.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            { // statement
                unique_ptr<Statement> statement_ptr = make_unique<Statement>(codeManager);
                if (statement_ptr->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(statement_ptr));
                else
                    return false;
            }
        }
    }
    return true ;
}
void StatementList::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type Statement::getType() const {
    return Type::STATEMENT ;
}
Statement::Statement(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool Statement::recursiveDescentParser(TokenStream& tokenStream) {
    if(tokenStream.isEmpty()) {
        codeManager->printCompileError(1 , "Return or Identifier token") ;
        return false ;
    }
    // "RETURN" additive-expression
    if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::KEYWORD) {
        TokenStream::Token token = tokenStream.lookup() ;
        string_view line = codeManager->getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
        size_t start_index = token.getCodeReference().getStartLineRange().second ;
        size_t last_index = token.getCodeReference().getEndLineRange().second ;
        string_view token_str = line.substr(start_index , last_index - start_index + 1) ;
        if(token_str == "RETURN") {
            { // "RETURN"
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,token.getCodeReference());
                children.emplace_back(move(genericToken));
                tokenStream.nextToken();
            }
            { // "additive-statement"
                unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager ) ;
                if(additiveExpression->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(additiveExpression)) ;
                else
                    return false ;
            }
        }
        else {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "RETURN statement or Identifier token") ;
            return false ;
        }
    }
    /// assignment-expression
    else if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::IDENTIFIER) {
        unique_ptr<AssignmentExpression> assignmentExpression = make_unique<AssignmentExpression>(codeManager) ;
        if(assignmentExpression->recursiveDescentParser(tokenStream))
            children.emplace_back(move(assignmentExpression)) ;
        else
            return false ;
    }
    else {
        codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "RETURN or Identifier Token") ;
        return false ;
    }
    return true ;
}
void Statement::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type AdditiveExpression::getType() const {
    return Type::ADDITIVE_EXPRESSION ;
}
AdditiveExpression::AdditiveExpression(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool AdditiveExpression::recursiveDescentParser(TokenStream& tokenStream) {
    { // multiplicative-expression
        unique_ptr<MultiplicativeExpression> multiplicativeExpression = make_unique<MultiplicativeExpression>(codeManager) ;
        if(multiplicativeExpression->recursiveDescentParser(tokenStream))
            children.emplace_back(move(multiplicativeExpression)) ;
        else {
            return false ;
        }
    }
    { // [('+' | '-') additive-expression]
        if(!tokenStream.isEmpty() && (tokenStream.lookup().getTokenType() == TokenStream::TokenType::PLUS_OPERATOR || tokenStream.lookup().getTokenType() == TokenStream::TokenType::MINUS_OPERATOR))
        {
            { //('+' | '-')
                TokenStream::Token operatorToken = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, operatorToken.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            { // additive-expression
                unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager);
                if (additiveExpression->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(additiveExpression));
                else {
                    return false;
                }
            }
        }
    }
    return true;
}
void AdditiveExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type MultiplicativeExpression::getType() const {
    return Type::MULTIPLICATIVE_EXPRESSION ;
}
MultiplicativeExpression::MultiplicativeExpression(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool MultiplicativeExpression::recursiveDescentParser(TokenStream& tokenStream) {
    { // unary-expression
        unique_ptr<UnaryExpression> unaryExpression = make_unique<UnaryExpression>(codeManager);
        if (unaryExpression->recursiveDescentParser(tokenStream))
            children.emplace_back(move(unaryExpression)) ;
        else
            return false ;
    }
    { // [('*' | '/') multiplicative-expression]
        if(!tokenStream.isEmpty() && (tokenStream.lookup().getTokenType() == TokenStream::TokenType::MULTIPLY_OPERATOR || tokenStream.lookup().getTokenType() == TokenStream::TokenType::DIVIDE_OPERATOR)) {
            { // ('*' | '/')
                TokenStream::Token operatorToken = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, operatorToken.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            { // multiplicative-expression
                unique_ptr<MultiplicativeExpression> multiplicativeExpression = make_unique<MultiplicativeExpression>(codeManager);
                if (multiplicativeExpression->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(multiplicativeExpression));
                else {
                    return false;
                }
            }
        }
    }
    return true ;
}
void MultiplicativeExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type AssignmentExpression::getType() const {
    return Type::ASSIGNMENT_EXPRESSION ;
}
AssignmentExpression::AssignmentExpression(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool AssignmentExpression::recursiveDescentParser(TokenStream& tokenStream) {
    { // identifier
        unique_ptr<Identifier> identifierToken = make_unique<Identifier>(codeManager) ;
        if(identifierToken->recursiveDescentParser(tokenStream))
            children.emplace_back(move(identifierToken)) ;
        else
        {
            return false ;
        }
    }
    { // ":= additive-expression"
        if(tokenStream.isEmpty()) {
            codeManager->printCompileError(2 , ":=") ;
            return false ;
        }
        else if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::VAR_ASSIGNMENT) {
            { // ":="
                TokenStream::Token token = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, token.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            { // "additive-expression"
                unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager);
                if (additiveExpression->recursiveDescentParser(tokenStream))
                    children.emplace_back(move(additiveExpression));
                else
                    return false;
            }
        }
        else {
            codeManager->printCompileError(tokenStream.lookup().getCodeReference(), ":=") ;
            return false ;
        }
    }
    return true ;
}
void AssignmentExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type UnaryExpression::getType() const {
    return Type::UNARY_EXPRESSION ;
}
UnaryExpression::UnaryExpression(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool UnaryExpression::recursiveDescentParser(TokenStream& tokenStream) {
    { // ['+' | '-']
        if ((tokenStream.lookup().getTokenType() == TokenStream::TokenType::PLUS_OPERATOR) || (tokenStream.lookup().getTokenType() == TokenStream::TokenType::MINUS_OPERATOR)) {
            TokenStream::Token token = tokenStream.nextToken();
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager, token.getCodeReference());
            children.emplace_back(move(genericToken));
        }
    }
    { // primary-expression
        unique_ptr<PrimaryExpression> primaryExpression = make_unique<PrimaryExpression>(codeManager);
        if (primaryExpression->recursiveDescentParser(tokenStream))
            children.emplace_back(move(primaryExpression));
        else {
            return false;
        }
    }
    return true ;
}
void UnaryExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type PrimaryExpression::getType() const {
    return Type::PRIMARY_EXPRESSION ;
}
PrimaryExpression::PrimaryExpression(management::CodeManager* manager) : NonTerminalNode(manager) {
}
bool PrimaryExpression::recursiveDescentParser(TokenStream& tokenStream) {
    if(tokenStream.isEmpty())
    {
        codeManager->printCompileError(1 , "Identifier , Literal or Open Bracket") ;
        return false ;
    }
    // identifier
    else if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::IDENTIFIER) {
        unique_ptr<Identifier> identifier = make_unique<Identifier>(codeManager) ;
        if(identifier->recursiveDescentParser(tokenStream))
            children.emplace_back(move(identifier)) ;
        else
            return false ;
    }
    // literal
    else if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::LITERAL) {
        unique_ptr<Literal> literal = make_unique<Literal>(codeManager) ;
        if(literal->recursiveDescentParser(tokenStream))
            children.emplace_back(move(literal)) ;
        else
            return false ;
    }
    // "(" additive-expression ")"
    else if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::OPEN_BRACKET) {
        { // "("
            TokenStream::Token open_bracket_token = tokenStream.nextToken() ;
            unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,open_bracket_token.getCodeReference());
            children.emplace_back(move(genericToken));
        }
        { // "additive-expression"
            unique_ptr<AdditiveExpression> additiveExpression = make_unique<AdditiveExpression>(codeManager) ;
            if(additiveExpression->recursiveDescentParser(tokenStream))
                children.emplace_back(move(additiveExpression)) ;
            else
                return false ;
        }
        { // ")"
            if(tokenStream.isEmpty()) {
                codeManager->printCompileError(1 , ")") ;
                return false ;
            }
            else if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::CLOSE_BRACKET) {
                TokenStream::Token close_bracket_token = tokenStream.nextToken();
                unique_ptr<GenericToken> genericToken = make_unique<GenericToken>(this->codeManager ,close_bracket_token.getCodeReference());
                children.emplace_back(move(genericToken));
            }
            else {
                codeManager->printCompileError(tokenStream.lookup().getCodeReference() , ")") ;
                return false ;
            }
        }
    }
    else {
        codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "Identifier , Literal or Open Bracket") ;
        return false ;
    }
    return true ;
}
void PrimaryExpression::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
ParseTreeNode::Type Identifier::getType() const {
    return Type::IDENTIFIER;
}
Identifier::Identifier(management::CodeManager* manager) : TerminalNode(manager) {
}
bool Identifier::recursiveDescentParser(TokenStream& tokenStream) {
    // identifier
    if(tokenStream.isEmpty()) {
        codeManager->printCompileError(1 , "Identifier Token") ;
        return false ;
    }
    if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::IDENTIFIER) {
        TokenStream::Token identifier_token = tokenStream.nextToken();
        this->codeReference = identifier_token.getCodeReference() ;
    }
    else {
        codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "Identifier Token") ;
        return false ;
    }
    return true ;
}
void Identifier::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
Literal::Literal(management::CodeManager* manager) : TerminalNode(manager) {
}
ParseTreeNode::Type Literal::getType() const {
    return Type::LITERAL ;
}
bool Literal::recursiveDescentParser(TokenStream& tokenStream) {
    // literal
    if(tokenStream.isEmpty()) {
        codeManager->printCompileError(1 , "Literal Token") ;
        return false ;
    }
    if(tokenStream.lookup().getTokenType() == TokenStream::TokenType::LITERAL) {
        TokenStream::Token literal = tokenStream.nextToken();
        this->codeReference = literal.getCodeReference();
    }
    else {
        codeManager->printCompileError(tokenStream.lookup().getCodeReference() , "Literal Token") ;
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
GenericToken::GenericToken(management::CodeManager* codeManager , management::CodeReference codeReference) : TerminalNode(codeManager , codeReference) {
    node_index = node_index_incrementer++ ;
}
bool GenericToken::recursiveDescentParser(TokenStream& /*tokenStream*/) {
    // generic token is created before recursiveDescentParser is called
    return true;
}
void GenericToken::accept(ParseTreeVisitor& parseTreeVisitor) const {
    parseTreeVisitor.visit(*this) ;
}
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------
