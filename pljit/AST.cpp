#include "pljit/AST.h"
#include "ASTVisitor.h"
#include "pljit/EvaluationContext.h"
#include "pljit/OptimizationASTVisitor.h"
#include <iostream>
#include <unordered_set>
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
    unique_ptr<ExpressionAST> analyzeExpression(const AdditiveExpression& additiveExpression) ;

    unique_ptr<ExpressionAST> analyzeExpression(const PrimaryExpression& primaryExpression)
    {
        if(primaryExpression.getChild(0).getType() == ParseTreeNode::Type::IDENTIFIER) {

            const Identifier& identifier = static_cast<const Identifier&>(primaryExpression.getChild(0)) ;
            return make_unique<IdentifierAST>(identifier.getManager() , identifier.getReference()) ;
        }
        else if(primaryExpression.getChild(0).getType() == ParseTreeNode::Type::LITERAL) {

            const Literal& literal = static_cast<const Literal&>(primaryExpression.getChild(0)) ;
            return make_unique<LiteralAST>(literal.getManager() , literal.getReference()) ;

        }
        const AdditiveExpression& additiveExpression = static_cast<const AdditiveExpression&>(primaryExpression.getChild(1)) ;
        return analyzeExpression(additiveExpression) ;
    }
    unique_ptr<ExpressionAST> analyzeExpression(const UnaryExpression& unaryExpression)
    {
        if(unaryExpression.num_children() == 2)
        {
            CodeManager* codeManager = unaryExpression.getManager() ;
            char op = (static_cast<const GenericToken&>(unaryExpression.getChild(0))).print_token()[0] ;
            const PrimaryExpression& primaryExpression = static_cast<const PrimaryExpression&>(unaryExpression.getChild(1));
            UnaryExpressionAST::UnaryType type = op == '+' ? UnaryExpressionAST::UnaryType::PLUS :
                                                             UnaryExpressionAST::UnaryType::MINUS;

            return make_unique<UnaryExpressionAST>(
                codeManager , unaryExpression.getReference() , type ,
                analyzeExpression(primaryExpression)
                                                   ) ;
        }
        const PrimaryExpression& primaryExpression = static_cast<const PrimaryExpression&>(unaryExpression.getChild(0));
        return analyzeExpression(primaryExpression) ;
    }

    unique_ptr<ExpressionAST> analyzeExpression(const MultiplicativeExpression& multiplicativeExpression) {
        const UnaryExpression& unaryExpression = static_cast<const UnaryExpression&>(multiplicativeExpression.getChild(0)) ;

        if(multiplicativeExpression.num_children() == 1)
            return analyzeExpression(unaryExpression) ;

        CodeManager* codeManager = multiplicativeExpression.getManager() ;
        char op = (static_cast<const GenericToken&>(multiplicativeExpression.getChild(1))).print_token()[0] ;
        const MultiplicativeExpression& anotherMultiplicativeExpression = static_cast<const MultiplicativeExpression&>(multiplicativeExpression.getChild(2)) ;
        BinaryExpressionAST::BinaryType type = op == '*' ? BinaryExpressionAST::BinaryType::MULTIPLY :
                                                           BinaryExpressionAST::BinaryType::DIVIDE;
        unique_ptr<BinaryExpressionAST> binaryExpressionAst = make_unique<BinaryExpressionAST>
                                                    (
                                                        codeManager ,
                                                        type ,
                                                        analyzeExpression(unaryExpression) ,
                                                        analyzeExpression(anotherMultiplicativeExpression)
                                                    );
        return binaryExpressionAst ;
    }
    unique_ptr<ExpressionAST> analyzeExpression(const AdditiveExpression& additiveExpression)
    {
        const MultiplicativeExpression& multiplicativeExpression = static_cast<const MultiplicativeExpression&>(additiveExpression.getChild(0)) ;

        if(additiveExpression.num_children() == 1)
            return analyzeExpression(multiplicativeExpression) ;

        CodeManager* codeManager = additiveExpression.getManager() ;
        char op = (static_cast<const GenericToken&>(additiveExpression.getChild(1))).print_token()[0] ;
        const AdditiveExpression& anotherAdditiveExpression = static_cast<const AdditiveExpression&>(additiveExpression.getChild(2)) ;
        BinaryExpressionAST::BinaryType type = op == '+' ? BinaryExpressionAST::BinaryType::PLUS :
                                                           BinaryExpressionAST::BinaryType::MINUS;
        unique_ptr<BinaryExpressionAST> binaryExpressionAst = make_unique<BinaryExpressionAST>(
            codeManager ,
            type ,
            analyzeExpression(multiplicativeExpression) ,
            analyzeExpression(anotherAdditiveExpression)
        ) ;
        return binaryExpressionAst ;
    }
    unique_ptr<StatementAST> analyzeStatement(const Statement& parseTreeNode)
    {
        CodeManager* codeManager = parseTreeNode.getManager() ;
        if(parseTreeNode.getChild(0).getType() == ParseTreeNode::Type::ASSIGNMENT_EXPRESSION)
        {
            const AssignmentExpression& assignmentExpression = static_cast<const AssignmentExpression&>(parseTreeNode.getChild(0)) ;
            const Identifier& identifier = static_cast<const Identifier&>(assignmentExpression.getChild(0)) ;
            const AdditiveExpression& additiveExpression = static_cast<const AdditiveExpression&>(assignmentExpression.getChild(2)) ;
            unique_ptr<AssignmentStatementAST> statementAst = make_unique<AssignmentStatementAST>
                (
                    codeManager ,
                    make_unique<IdentifierAST>(identifier.getManager() , identifier.getReference()) ,
                    analyzeExpression(additiveExpression)
                 )
            ;
            return statementAst ;
        }
        const AdditiveExpression& additiveExpression = static_cast<const AdditiveExpression&>(parseTreeNode.getChild(1)) ;
        unique_ptr<ReturnStatementAST> statementAst = make_unique<ReturnStatementAST>
            (
                codeManager ,
                analyzeExpression(additiveExpression)
                );
        return statementAst ;
    }
} // anonymous namespace

size_t ASTNode :: node_index_incrementer {0};

bool SymbolTable::addAttributes(const ParameterDeclaration& declaration) {

    const DeclartorList& declartorList = static_cast<const DeclartorList&>(declaration.getChild(1)) ;
    size_t parameter_index = 0 ;
    for(size_t index = 0 ; index < declartorList.num_children() ; ++index) {
        const TerminalNode& curChild = static_cast<const TerminalNode&>(declartorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::IDENTIFIER) {
            if(!this->isDeclared(curChild.print_token())) {
                this->insert(curChild.print_token(), AttributeType::PARAMETER ,curChild.getReference() , parameter_index , nullopt);
                parameter_index++ ;
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
    size_t variable_index = 0 ;
    for(size_t index = 0 ; index < declartorList.num_children() ; ++index) {
        const TerminalNode& curChild = static_cast<const TerminalNode&>(declartorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::IDENTIFIER) {
            if(!this->isDeclared(curChild.print_token()))
                this->insert(curChild.print_token() , AttributeType::VARIABLE, curChild.getReference() , variable_index++ , nullopt) ;
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
    size_t constant_index = 0 ;
    for(size_t index = 0 ; index < declartorList.num_children() ; ++index) {
        const ParseTreeNode& curChild = static_cast<const ParseTreeNode&>(declartorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::INIT_DECLARATOR) {
            const InitDeclartor& init_declarator = static_cast<const InitDeclartor&>(curChild);
            const Identifier& identifier = static_cast<const Identifier&>(init_declarator.getChild(0)) ;
            const Literal& literal = static_cast<const Literal&>(init_declarator.getChild(2)) ;
            int64_t value = str_to_int64(literal.print_token()) ;
            if(!isDeclared(identifier.print_token())) {
                insert(identifier.print_token() , AttributeType::CONSTANT, curChild.getReference() , constant_index , value);
                constant_index++ ;
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
SymbolTable::SymbolTable(const FunctionDeclaration& functionDeclaration) {
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
bool SymbolTable::isDeclared(std::string_view identifier) const {
    return tableIdentifiers.find(identifier) != tableIdentifiers.end() ;
}

bool SymbolTable::isConstant(std::string_view identifier)  {
    return  get<0>(tableIdentifiers[identifier]) == AttributeType::CONSTANT;
}
bool SymbolTable::isComplied() const {
    return isCompiled ;
}
void SymbolTable::insert(std::string_view identifier , AttributeType type ,CodeReference codeReference , size_t index , std::optional<int64_t> value) {
    assert(!isDeclared(identifier)) ;
    tableIdentifiers[identifier] = make_tuple(type , codeReference , index , value) ;
}
const unordered_map<std::string_view, std::tuple<SymbolTable::AttributeType, CodeReference, size_t, std::optional<int64_t>>>& SymbolTable::getTableContent() const {
    return tableIdentifiers ;
}

//---------------------------------------------------------------------------
ASTNode::Type FunctionAST::getType() const{
    return ASTNode::Type::FUNCTION ;
}
FunctionAST::FunctionAST(std::unique_ptr<FunctionDeclaration> &functionDeclaration , CodeManager* manager) {
    this->codeManager = manager ;
    node_index = node_index_incrementer++ ;

    symbolTable = make_unique<SymbolTable>(*functionDeclaration) ;
    size_t compound_index = 0 ;
    for(size_t index = 0 ; index < functionDeclaration->num_children() ; index++)
    {
        const ParseTreeNode& curChild = functionDeclaration->getChild(index) ;
        if(curChild.getType() == ParseTreeNode::Type::COMPOUND_STATEMENT)
        {
            compound_index = index ;
            break ;
        }
    }
    const CompoundStatement& compoundStatement = static_cast<const CompoundStatement&>(functionDeclaration->getChild(compound_index));
    const StatementList& statementList = static_cast<const StatementList&>(compoundStatement.getChild(1));
    unordered_set<string_view> initializedVariables ; // TODO Handling uninitialized variables
    bool returnStatementTriggered = false ;
    for(size_t statement_index = 0 ; statement_index < statementList.num_children() ; statement_index++)
    {
        const ParseTreeNode& curChild = statementList.getChild(statement_index) ;
        if(curChild .getType() == ParseTreeNode::Type::STATEMENT)
        {
            const Statement &statement = static_cast<const Statement&>(statementList.getChild(statement_index));
            children.emplace_back(analyzeStatement(statement)) ;
            if(children.front().get()->getType() == ASTNode::Type::RETURN_STATEMENT)
                returnStatementTriggered = true ;
        }
    }
    if(!returnStatementTriggered)
    {
        // TODO missing return statement ;
//        manager->
    }
}
const StatementAST& FunctionAST::getStatement(size_t index) const {
    return *children[index] ;
}
std::size_t FunctionAST::statement_size() const {
    return children.size() ;
}
std::unique_ptr<StatementAST> FunctionAST::releaseStatement(size_t index) {
    unique_ptr<StatementAST> node = move(children[index]);
    return node ;
}
void FunctionAST::accept(ASTVisitor& astVistor) const
{
    astVistor.visit(*this) ;
}
std::optional<int64_t> FunctionAST::evaluate(EvaluationContext& evaluationContext) const {

    for(const unique_ptr<StatementAST> & statementAst : children)
    {
        if(statementAst->getType() == ASTNode::Type::RETURN_STATEMENT)
            return statementAst->evaluate(evaluationContext) ;
        else {
            const AssignmentStatementAST& curr = static_cast<const AssignmentStatementAST&>(*statementAst.get()) ;
            string_view identifier = curr.getLeftIdentifier().print_token() ;
            optional<int64_t> result = statementAst->evaluate(evaluationContext) ;
            if(result)
                evaluationContext.updateIdentifier(identifier , result.value());
            else
                return nullopt ;
        }
    }
    return nullopt ;
}
std::optional<int64_t> FunctionAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}
//void FunctionAST::optimize(EvaluationContext& evaluationContext, optional<int64_t>& evaluatedValue) {
//
//    size_t statement_size = 0 ;
//    for(unique_ptr<StatementAST> & currStatement : this->children) {
//        std::optional<int64_t> assignmentValue = nullopt ;
//        currStatement->optimize(currStatement , evaluationContext , assignmentValue) ;
//        statement_size ++ ;
//        if(currStatement->getType() == ASTNode::Type::RETURN_STATEMENT) {
//
//            if(assignmentValue) {
////                unique_ptr<ExpressionAST> input = make_unique<LiteralAST>(assignmentValue.value()) ;
//                unique_ptr<ReturnStatementAST> returnStatementAst = make_unique<ReturnStatementAST>(codeManager , make_unique<LiteralAST>(assignmentValue.value())) ;
//                children.clear() ;
//                children.emplace_back(move(returnStatementAst)) ;
//                evaluatedValue = assignmentValue ;
//            }
//            else
//                children.resize(statement_size) ;
//            return ;
//        }
//        else {
//            unique_ptr<AssignmentStatementAST>assignmentStatement(static_cast<AssignmentStatementAST*>(currStatement.get()));
//            if(assignmentValue) {
//                evaluationContext.updateIdentifier(assignmentStatement->getLeftIdentifier().print_token() , assignmentValue.value()) ;
//            }
//        }
//    }
//}

ASTNode::Type AssignmentStatementAST::getType() const {
    return ASTNode::Type::ASSIGNMENT_STATEMENT;
}
AssignmentStatementAST::AssignmentStatementAST(CodeManager* manager) : StatementAST(manager) {

}
AssignmentStatementAST::AssignmentStatementAST(CodeManager* manager, std::unique_ptr<IdentifierAST> left, std::unique_ptr<ExpressionAST> right) : AssignmentStatementAST(manager){
    this->leftIdentifier = move(left) ;
    this->rightExpression = move(right) ;
}
const IdentifierAST& AssignmentStatementAST::getLeftIdentifier() const {
    return *leftIdentifier ;
}
std::unique_ptr<IdentifierAST> AssignmentStatementAST::releaseLeftIdentifier() {
    unique_ptr<IdentifierAST> node = move(leftIdentifier) ;
    return node ;
}
const ExpressionAST& AssignmentStatementAST::getRightExpression() const {
    return *rightExpression ;
}
std::unique_ptr<ExpressionAST> AssignmentStatementAST::releaseRightExpression() {
    unique_ptr<ExpressionAST> node = move(rightExpression) ;
    return node ;
}
void AssignmentStatementAST::accept(ASTVisitor& astVistor) const {
    astVistor.visit(*this) ;
}
std::optional<int64_t> AssignmentStatementAST::evaluate(EvaluationContext& evaluationContext) const {
    return rightExpression->evaluate(evaluationContext);
}
std::optional<int64_t> AssignmentStatementAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}

ASTNode::Type ReturnStatementAST::getType() const {
    return ASTNode::Type::RETURN_STATEMENT;
}
ReturnStatementAST::ReturnStatementAST(CodeManager* manager, std::unique_ptr<ExpressionAST> input) : StatementAST(manager)
{
    this->input = move(input) ;
}
const ExpressionAST& ReturnStatementAST::getInput() const {
    return *input.get() ;
}
std::unique_ptr<ExpressionAST> ReturnStatementAST::releaseInput() {
    unique_ptr<ExpressionAST> node = move(input) ;
    return node ;
}
void ReturnStatementAST::accept(ASTVisitor& astVistor) const {
    astVistor.visit(*this) ;
}
std::optional<int64_t> ReturnStatementAST::evaluate(EvaluationContext& evaluationContext) const {
    return input->evaluate(evaluationContext);
}
std::optional<int64_t> ReturnStatementAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}

ASTNode::Type BinaryExpressionAST::getType() const {
    return ASTNode::Type::BINARY_EXPRESSION;
}
BinaryExpressionAST::BinaryExpressionAST(CodeManager* manager, BinaryExpressionAST::BinaryType type, std::unique_ptr<ExpressionAST> left, std::unique_ptr<ExpressionAST> right) : ExpressionAST(manager){
    this->binaryType = type ;
    this->leftExpression = move(left) ;
    this->rightExpression = move(right) ;
}
BinaryExpressionAST::BinaryType BinaryExpressionAST::getBinaryType() const {
    return binaryType ;
}
const ExpressionAST& BinaryExpressionAST::getLeftExpression() const {
    return *leftExpression.get() ;
}
std::unique_ptr<ExpressionAST> BinaryExpressionAST::releaseLeftExpression() {
    unique_ptr<ExpressionAST> node = move(leftExpression) ;
    return node ;
}
const ExpressionAST& BinaryExpressionAST::getRightExpression() const {
    return *rightExpression.get() ;
}
std::unique_ptr<ExpressionAST> BinaryExpressionAST::releaseRightExpression() {
    unique_ptr<ExpressionAST> node = move(rightExpression) ;
    return node ;

}
void BinaryExpressionAST::accept(ASTVisitor& astVistor) const {
    astVistor.visit(*this) ;
}
std::optional<int64_t> BinaryExpressionAST::evaluate(EvaluationContext& evaluationContext) const {

    optional<int64_t> leftResult = leftExpression->evaluate(evaluationContext) ;
    optional<int64_t> rightResult = rightExpression->evaluate(evaluationContext) ;
    if(!leftResult || !rightResult)
        return nullopt ;
    switch (getBinaryType()) {
        case BinaryType::PLUS: return leftResult.value() + rightResult.value();
        case BinaryType::MINUS: return leftResult.value() - rightResult.value();
        case BinaryType::MULTIPLY: return leftResult.value() * rightResult.value();
        case BinaryType::DIVIDE:
        {
            // TODO error for division by zero
            return leftResult.value() / rightResult.value() ;
        };
    }
    return nullopt;
}
std::optional<int64_t> BinaryExpressionAST::acceptOptimization(OptimizationVisitor& astVisitor) {
    return astVisitor.visitOptimization(*this) ;
}

ASTNode::Type UnaryExpressionAST::getType() const {
    return ASTNode::Type::UNARY_EXPRESSION;
}
UnaryExpressionAST::UnaryExpressionAST(CodeManager* manager, CodeReference codeReference1, UnaryExpressionAST::UnaryType type, std::unique_ptr<ExpressionAST> input) : ExpressionAST(manager , codeReference1){
    this->unaryType = type ;
    this->input = move(input);
}
UnaryExpressionAST::UnaryType UnaryExpressionAST::getUnaryType() const {
    return unaryType ;
}
const ExpressionAST& UnaryExpressionAST::getInput() const {
    return *input.get() ;
}
std::unique_ptr<ExpressionAST> UnaryExpressionAST::releaseInput() {
    unique_ptr<ExpressionAST> node = move(input) ;
    return node ;
}
void UnaryExpressionAST::accept(ASTVisitor& astVistor) const {
    astVistor.visit(*this) ;
}
std::optional<int64_t> UnaryExpressionAST::evaluate(EvaluationContext& evaluationContext) const {
    optional<int64_t> result = input->evaluate(evaluationContext) ;
    if(getUnaryType() == UnaryType::MINUS)
        return -result.value() ;
    return result ;
}
std::optional<int64_t> UnaryExpressionAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}

ASTNode::Type IdentifierAST::getType() const {
    return ASTNode::Type::IDENTIFIER;
}
IdentifierAST::IdentifierAST(CodeManager* manager, CodeReference codeReference) : ExpressionAST(manager, codeReference) {

}
std::string_view IdentifierAST::print_token() const {
    size_t line  = codeReference.getLineRange().first ;
    size_t begin = codeReference.getStartLineRange().first ;
    size_t last = codeReference.getStartLineRange().second ;
    return codeManager->getCurrentLine(line).substr(begin , last - begin + 1) ;
}
void IdentifierAST::accept(ASTVisitor& astVistor) const {
    astVistor.visit(*this) ;
}
std::optional<int64_t> IdentifierAST::evaluate(EvaluationContext& evaluationContext) const {
    return evaluationContext.getIdentifier(print_token());
}
std::optional<int64_t> IdentifierAST::acceptOptimization(OptimizationVisitor& astVisitor) {
    return astVisitor.visitOptimization(*this) ;
}

ASTNode::Type LiteralAST::getType() const {
    return ASTNode::Type::LITERAL;
}
LiteralAST::LiteralAST(CodeManager* manager, CodeReference codeReference) : ExpressionAST(manager, codeReference) {
     size_t line  = codeReference.getLineRange().first ;
     size_t begin = codeReference.getStartLineRange().first ;
     size_t last = codeReference.getStartLineRange().second ;
     string_view token = codeManager->getCurrentLine(line).substr(begin , last - begin + 1) ;
     value = str_to_int64(token) ;
}
void LiteralAST::accept(ASTVisitor& astVistor) const {
    astVistor.visit(*this) ;
}
std::optional<int64_t> LiteralAST::evaluate(EvaluationContext& /*evaluationContext*/) const {
    return value ;
}
int64_t LiteralAST::getValue() const {
    return value ;
}
LiteralAST::LiteralAST(int64_t value) : value(value){
}
std::optional<int64_t> LiteralAST::acceptOptimization(OptimizationVisitor& astVisitor) {
    return astVisitor.visitOptimization(*this) ;
}

size_t ASTNode::getNodeID() const{
    return node_index;
}
CodeReference ASTNode::getReference() const {
    return codeReference;
}
const SymbolTable& ASTNode::getSymbolTable() const {
    return *symbolTable ;
}
ASTNode::~ASTNode() = default ;

StatementAST::StatementAST() {
    node_index = node_index_incrementer++ ;
}
StatementAST::StatementAST(CodeManager* manager) : StatementAST(){
    this->codeManager = manager ;
}
StatementAST::StatementAST(CodeManager* manager, CodeReference codeReference1) : StatementAST(manager) {
    this->codeReference = codeReference1 ;
}
ExpressionAST::ExpressionAST() {
    node_index = node_index_incrementer++ ;
}
ExpressionAST::ExpressionAST(CodeManager* manager):ExpressionAST() {
    this->codeManager = manager ;
}
ExpressionAST::ExpressionAST(CodeManager* manager, CodeReference codeReference1) : ExpressionAST(manager){
    this->codeReference = codeReference1 ;
}
} //namespace jitcompiler
//---------------------------------------------------------------------------
