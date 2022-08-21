#include "pljit/semantic/AST.hpp"

#include "pljit/semantic/ASTVisitor.hpp"
#include "pljit/semantic/PrintASTVisitor.hpp"
#include "pljit/semantic/EvaluationContext.hpp"
#include "pljit/semantic/OptimizationASTVisitor.hpp"
//---------------------------------------------------------------------------
#include <unordered_set>
//---------------------------------------------------------------------------
using namespace std ;
using namespace jitcompiler::syntax ;
//---------------------------------------------------------------------------
namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------
//helper functions
namespace {
//---------------------------------------------------------------------------

    int64_t str_to_int64(string_view literal)
    /// convert string literal to 64 bits signed integer
    {
        int64_t value = 0 ;
        for(char c : literal) {
            assert('0' <= c && c <= '9') ;
            value = value * 10 + static_cast<int64_t>(c - '0') ;
        }
        return value ;
    }
    //---------------------------------------------------------------------------
    // nullptr if semantic error is triggered
    unique_ptr<ExpressionAST> analyzeExpression(const AdditiveExpression& additiveExpression , const SymbolTable& symbolTable , const unordered_set<string_view> &initializedVariables) ;
    //---------------------------------------------------------------------------
    // nullptr if semantic error is triggered
    unique_ptr<ExpressionAST> analyzeExpression(const PrimaryExpression& primaryExpression , const SymbolTable& symbolTable , const unordered_set<string_view> &initializedVariables)
    // analyze primary expression of parse tree node to return ASTNode of an expression
    {
        // size(primary expression) = 1 for Identifier and Literal and = 3 for "(" additive-expression ")"

        if(primaryExpression.getChild(0).getType() == ParseTreeNode::Type::IDENTIFIER)
        // if primary expression is identifier
        {

            const Identifier& identifier = static_cast<const Identifier&>(primaryExpression.getChild(0)) ;
            management::CodeManager* manager = identifier.getManager() ;
            if(!symbolTable.isDeclared(identifier.print_token()))
            // trigger undeclared identifier
            {
                manager->printSemanticError(identifier.getReference() , "Undeclared Identifier") ;
                return nullptr;
            }
            else if(symbolTable.isVariable(identifier.print_token()) && !initializedVariables.contains(identifier.print_token()))
            // trigger uninitialized identifier
            {
                manager->printSemanticError(identifier.getReference() , "Uninitialized Identifier") ;
                return nullptr ;
            }
            return make_unique<IdentifierAST>(identifier.getManager() , identifier.getReference()) ;
        }
        else if(primaryExpression.getChild(0).getType() == ParseTreeNode::Type::LITERAL)
        // if primary expression is identifier
        {

            const Literal& literal = static_cast<const Literal&>(primaryExpression.getChild(0)) ;
            return make_unique<LiteralAST>(literal.getManager() , literal.getReference()) ;
        }
        //  primary expression is additive-expression
        const AdditiveExpression& additiveExpression = static_cast<const AdditiveExpression&>(primaryExpression.getChild(1)) ;
        return analyzeExpression(additiveExpression , symbolTable , initializedVariables) ;
    }
    //---------------------------------------------------------------------------
    // nullptr if semantic error is triggered
    unique_ptr<ExpressionAST> analyzeExpression(const UnaryExpression& unaryExpression , const SymbolTable& symbolTable , const unordered_set<string_view> &initializedVariables)
    {
        if(unaryExpression.num_children() == 2)
        // there is a unary operator
        {
            const PrimaryExpression& primaryExpression = static_cast<const PrimaryExpression&>(unaryExpression.getChild(1));
            auto child = analyzeExpression(primaryExpression , symbolTable , initializedVariables) ;
            if(child == nullptr)
                return nullptr ;
            const syntax::GenericToken& genericToken = static_cast<const GenericToken&>(unaryExpression.getChild(0)) ;
            UnaryExpressionAST::UnaryType type = genericToken.print_token()[0]  == '+' ?
                                UnaryExpressionAST::UnaryType::PLUS :UnaryExpressionAST::UnaryType::MINUS;
            return make_unique<UnaryExpressionAST>(
                unaryExpression.getManager() , genericToken.getReference() , type ,
                move(child)) ;
        }
        // only primary expression without unary operator
        const PrimaryExpression& primaryExpression = static_cast<const PrimaryExpression&>(unaryExpression.getChild(0));
        return analyzeExpression(primaryExpression , symbolTable , initializedVariables) ;
    }
    //---------------------------------------------------------------------------
    // nullptr if semantic error is triggered
    unique_ptr<ExpressionAST> analyzeExpression(const MultiplicativeExpression& multiplicativeExpression ,const SymbolTable& symbolTable , const unordered_set<string_view> &initializedVariables) {
        const UnaryExpression& unaryExpression = static_cast<const UnaryExpression&>(multiplicativeExpression.getChild(0)) ;
        // if there is no binary operator ("*" , "/")
        if(multiplicativeExpression.num_children() == 1)
            return analyzeExpression(unaryExpression , symbolTable , initializedVariables) ;

        // there is a binary operator ("*" , "/")
        const MultiplicativeExpression& anotherMultiplicativeExpression = static_cast<const MultiplicativeExpression&>(multiplicativeExpression.getChild(2)) ;

        // analyze left and right child recursively
        auto leftChild = analyzeExpression(unaryExpression , symbolTable , initializedVariables) ;
        if(leftChild == nullptr)
            return nullptr ;
        auto rightChild = analyzeExpression(anotherMultiplicativeExpression , symbolTable , initializedVariables) ;
        if(rightChild == nullptr)
            return nullptr ;


        const GenericToken& tokenOperator = static_cast<const GenericToken&>(multiplicativeExpression.getChild(1)) ;
        char op = tokenOperator.print_token()[0] ;
        BinaryExpressionAST::BinaryType type = op == '*' ? BinaryExpressionAST::BinaryType::MULTIPLY :
                                                           BinaryExpressionAST::BinaryType::DIVIDE;
        return make_unique<BinaryExpressionAST>
            (
                multiplicativeExpression.getManager() ,
                type , move(leftChild)
                          ,  move(rightChild) , tokenOperator.getReference()
            ) ;
    }
    //---------------------------------------------------------------------------
    // nullptr if semantic error is triggered
    unique_ptr<ExpressionAST> analyzeExpression(const AdditiveExpression& additiveExpression , const SymbolTable& symbolTable , const unordered_set<string_view> &initializedVariables)
    {
        const MultiplicativeExpression& multiplicativeExpression = static_cast<const MultiplicativeExpression&>(additiveExpression.getChild(0)) ;

        // if there is no binary operator ("+" , "-")
        if(additiveExpression.num_children() == 1)
            return analyzeExpression(multiplicativeExpression , symbolTable ,initializedVariables) ;
        // there is a binary operator ("*" , "/")

        const AdditiveExpression& anotherAdditiveExpression = static_cast<const AdditiveExpression&>(additiveExpression.getChild(2)) ;

        // analyze left and right child recursively
        auto leftChild = analyzeExpression(multiplicativeExpression , symbolTable , initializedVariables)  ;
        if(leftChild == nullptr)
            return nullptr ;
        auto rightChild = analyzeExpression(anotherAdditiveExpression , symbolTable , initializedVariables) ;
        if(rightChild == nullptr)
            return nullptr ;

        const GenericToken& tokenOperator = static_cast<const GenericToken&>(additiveExpression.getChild(1)) ;
        BinaryExpressionAST::BinaryType type = tokenOperator.print_token()[0]  == '+' ? BinaryExpressionAST::BinaryType::PLUS :
                                                           BinaryExpressionAST::BinaryType::MINUS;

        return make_unique<BinaryExpressionAST>(
            additiveExpression.getManager() ,
            type ,
            move(leftChild) ,
            move(rightChild)
        ) ;
    }
    //---------------------------------------------------------------------------
    // nullptr if semantic error is triggered
    unique_ptr<StatementAST> analyzeStatement(const Statement& parseTreeNode , const SymbolTable& symbolTable, unordered_set<string_view> &initializedVariables)
    {
        management::CodeManager* codeManager = parseTreeNode.getManager() ;
        if(parseTreeNode.getChild(0).getType() == ParseTreeNode::Type::ASSIGNMENT_EXPRESSION)
        // if statement is assignment statement
        {
            const AssignmentExpression& assignmentExpression = static_cast<const AssignmentExpression&>(parseTreeNode.getChild(0)) ;
            const Identifier& identifier = static_cast<const Identifier&>(assignmentExpression.getChild(0)) ;
            if(!symbolTable.isDeclared(identifier.print_token()))
                // if left-side identifier is undeclared variable , trigger semantic error
            {
                identifier.getManager()->printSemanticError(identifier.getReference() , "Undeclared Identifier") ;
                return nullptr ;
            }
            else if(symbolTable.isConstant(identifier.print_token()))
            // if left-side identifier is constant , trigger semantic error
            {
                identifier.getManager()->printSemanticError(identifier.getReference() , "Constant Assignment") ;
                return nullptr ;
            }
            // analyze right expression recursively
            const AdditiveExpression& additiveExpression = static_cast<const AdditiveExpression&>(assignmentExpression.getChild(2)) ;
            auto rightExpression = analyzeExpression(additiveExpression , symbolTable , initializedVariables) ;
            if(rightExpression == nullptr)
                return nullptr ;
            if(symbolTable.isVariable(identifier.print_token()))
                initializedVariables.insert(identifier.print_token()) ;
            return make_unique<AssignmentStatementAST>
                (
                    codeManager ,
                    make_unique<IdentifierAST>(identifier.getManager() , identifier.getReference()) ,
                    move(rightExpression)
                ) ;
        }
        // Return statement
        const AdditiveExpression& additiveExpression = static_cast<const AdditiveExpression&>(parseTreeNode.getChild(1)) ;

        // analyze expression of return statement recursively
        auto child = analyzeExpression(additiveExpression , symbolTable , initializedVariables) ;
        if(child == nullptr)
            return nullptr ;
        return make_unique<ReturnStatementAST>
            (
                codeManager ,
                move(child)
            );
    }
//---------------------------------------------------------------------------
} // anonymous namespace
//---------------------------------------------------------------------------
size_t ASTNode :: node_index_incrementer {0};
//---------------------------------------------------------------------------
bool SymbolTable::addAttributes(const ParameterDeclaration& declaration) {

    const DeclaratorList& declaratorList = static_cast<const DeclaratorList&>(declaration.getChild(1)) ;
    size_t parameter_index = 0 ;
    for(size_t index = 0 ; index < declaratorList.num_children() ; ++index) {
        const TerminalNode& curChild = static_cast<const TerminalNode&>(declaratorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::IDENTIFIER) {
            if(!this->isDeclared(curChild.print_token())) {
                this->insert(curChild.print_token(), AttributeType::PARAMETER ,curChild.getReference() , parameter_index , nullopt);
                parameter_index++ ;
            }
            else {
                codeManager->printSemanticError(curChild.getReference() , "Already declared") ;
                isCompiled = false ;
                return false ;
            }
        }
    }
    return true ;
}
//---------------------------------------------------------------------------
bool SymbolTable::addAttributes(const VariableDeclaration& declaration) {
    const DeclaratorList& declaratorList = static_cast<const DeclaratorList&>(declaration.getChild(1)) ;
    for(size_t index = 0 ; index < declaratorList.num_children() ; ++index) {
        const TerminalNode& curChild = static_cast<const TerminalNode&>(declaratorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::IDENTIFIER) {
            if(!this->isDeclared(curChild.print_token()))
                this->insert(curChild.print_token() , AttributeType::VARIABLE, curChild.getReference() , 0 , nullopt) ;
            else {
                codeManager->printSemanticError(curChild.getReference() , "Already declared") ;
                isCompiled = false ;
                return false ;
            }
        }
    }
    return true ;
}
//---------------------------------------------------------------------------
bool SymbolTable::addAttributes(const ConstantDeclaration& declaration) {
    const InitDeclaratorList& declaratorList = static_cast<const InitDeclaratorList&>(declaration.getChild(1)) ;
    for(size_t index = 0 ; index < declaratorList.num_children() ; ++index) {
        const ParseTreeNode& curChild = static_cast<const ParseTreeNode&>(declaratorList.getChild(index)) ;
        if (curChild.getType() == ParseTreeNode::Type::INIT_DECLARATOR) {
            const InitDeclarator& init_declarator = static_cast<const InitDeclarator&>(curChild);
            const Identifier& identifier = static_cast<const Identifier&>(init_declarator.getChild(0)) ;
            const Literal& literal = static_cast<const Literal&>(init_declarator.getChild(2)) ;
            int64_t value = str_to_int64(literal.print_token()) ;
            if(!isDeclared(identifier.print_token())) {
                insert(identifier.print_token() , AttributeType::CONSTANT, identifier.getReference() , 0 , value);
            }
            else {
                codeManager->printSemanticError(identifier.getReference() , "Already declared") ;
                isCompiled = false ;
                return false ;
            }
        }
    }
    return true ;
}
//---------------------------------------------------------------------------
SymbolTable::SymbolTable(management::CodeManager* codeManager , const FunctionDeclaration& functionDeclaration) {
    this->codeManager = codeManager ;
    for(size_t index = 0 ; isCompiled  && index < functionDeclaration.num_children(); ++index) {
        const ParseTreeNode& curNode = functionDeclaration.getChild(index) ;
        if(curNode.getType() == ParseTreeNode::Type::PARAMETER_DECLARATION) {
            const ParameterDeclaration& declaration = static_cast<const ParameterDeclaration&>(curNode) ;
            isCompiled &= addAttributes(declaration) ;
        }
        else if(curNode.getType() == ParseTreeNode::Type::VARIABLE_DECLARATION) {
            const VariableDeclaration& declaration = static_cast<const VariableDeclaration&>(curNode) ;
            isCompiled &= addAttributes(declaration) ;
        }
        else if(curNode.getType() == ParseTreeNode::Type::CONSTANT_DECLARATION) {
            const ConstantDeclaration& declaration = static_cast<const ConstantDeclaration&>(curNode) ;
            isCompiled &= addAttributes(declaration) ;
        }
    }
}
//---------------------------------------------------------------------------
bool SymbolTable::isDeclared(std::string_view identifier) const {
    return tableIdentifier[PARAMETER].find(identifier) != tableIdentifier[PARAMETER].end()
            || tableIdentifier[VARIABLE].find(identifier) != tableIdentifier[VARIABLE].end()
            || tableIdentifier[CONSTANT].find(identifier) != tableIdentifier[CONSTANT].end() ;
}
//---------------------------------------------------------------------------
bool SymbolTable::isConstant(std::string_view identifier) const {
    return  tableIdentifier[CONSTANT].find(identifier) != tableIdentifier[CONSTANT].end() ;
}
//---------------------------------------------------------------------------
bool SymbolTable::isVariable(std::string_view identifier) const {
    return  tableIdentifier[VARIABLE].find(identifier) != tableIdentifier[VARIABLE].end() ;
}
//---------------------------------------------------------------------------
bool SymbolTable::isComplied() const {
    return isCompiled ;
}
//---------------------------------------------------------------------------
void SymbolTable::insert(std::string_view identifier , AttributeType type ,management::CodeReference codeReference , size_t index , std::optional<int64_t> value) {
    assert(!isDeclared(identifier)) ;
    tableIdentifier[type][identifier] = make_tuple(codeReference , index , value) ;
}
//---------------------------------------------------------------------------
const array<unordered_map<std::string_view, std::tuple<management::CodeReference, size_t, std::optional<int64_t>>> , 3>& SymbolTable::getTableContent() {
    return tableIdentifier ;
}
SymbolTable::SymbolTable() = default ;
//---------------------------------------------------------------------------
ASTNode::ASTType FunctionAST::getAstType() const{
    return ASTNode::ASTType::FUNCTION ;
}
//---------------------------------------------------------------------------
FunctionAST::FunctionAST(management::CodeManager* manager) {
    this->codeManager = manager ;
    node_index = node_index_incrementer++ ;
}
//---------------------------------------------------------------------------
bool FunctionAST::compileCode(const FunctionDeclaration& functionDeclaration) {
    // add declarations
    symbolTable = SymbolTable(codeManager , functionDeclaration) ;
    // check if declarations compiled
    if(!symbolTable.isComplied())
        return false ;
    optional<size_t> compound_index ;
    for(size_t index = 0 ; index < functionDeclaration.num_children() ; index++)
    {
        const ParseTreeNode& curChild = functionDeclaration.getChild(index) ;
        if(curChild.getType() == ParseTreeNode::Type::COMPOUND_STATEMENT)
        {
            compound_index = index ;
            break ;
        }
    }
    // assertion for compound statement (must be triggered in parse tree as compile error)
    assert(compound_index.has_value()) ;

    const CompoundStatement& compoundStatement = static_cast<const CompoundStatement&>(functionDeclaration.getChild(compound_index.value()));
    const StatementList& statementList = static_cast<const StatementList&>(compoundStatement.getChild(1));

    bool returnStatementTriggered = false ;
    // to check for each statement over each statement => to trigger error (using uninitialized variable) for variable declarations only
    unordered_set<string_view> initializedVariables ;

    for(size_t statement_index = 0 ; statement_index < statementList.num_children() ; statement_index++) {
        const ParseTreeNode& curChild = statementList.getChild(statement_index) ;
        if(curChild .getType() == ParseTreeNode::Type::STATEMENT)
        // iterate over all statements
        {
            const Statement &statement = static_cast<const Statement&>(statementList.getChild(statement_index));
            // analyze each statement recursively and get ASTNode of current statement
            auto child = analyzeStatement(statement , symbolTable , initializedVariables) ;
            if(child == nullptr)
                return false ;

            returnStatementTriggered |= child->getAstType() == ASTNode::ASTType::RETURN_STATEMENT ;

            children.emplace_back(move(child)) ;
        }
    }
    if(!returnStatementTriggered)
        // missing return statement => semantic error
    {
        const syntax::GenericToken& endToken = static_cast<const GenericToken&>(compoundStatement.getChild(2)) ;
        management::CodeReference endReference = endToken.getReference();
        codeManager->printSemanticError(endReference , "Missing Return Statement") ;
        return false ;
    }
    return true;
}
//---------------------------------------------------------------------------
const StatementAST& FunctionAST::getStatement(size_t index) const {
    return *children[index] ;
}
//---------------------------------------------------------------------------
std::size_t FunctionAST::num_statements() const {
    return children.size() ;
}
//---------------------------------------------------------------------------
void FunctionAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> FunctionAST::evaluate(EvaluationContext& evaluationContext) const {

    for(const unique_ptr<StatementAST> & statementAst : children)
    /// iterate over each statement
    {
        /// return evaluation if return statement
        if(statementAst->getAstType() == ASTNode::ASTType::RETURN_STATEMENT)
            return statementAst->evaluate(evaluationContext) ;
        else
            // evaluate right expression of assignment statement then store it to LHS identifier
        {
            const AssignmentStatementAST& curr = static_cast<const AssignmentStatementAST&>(*statementAst) ;
            string_view identifier = curr.getLeftIdentifier().print_token() ;
            optional<int64_t> result = statementAst->evaluate(evaluationContext) ;
            if(result.has_value())
                evaluationContext.updateIdentifier(identifier , result.value());
            else
                return nullopt ;
        }
    }
    return nullopt ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> FunctionAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
ASTNode::ASTType AssignmentStatementAST::getAstType() const {
    return ASTNode::ASTType::ASSIGNMENT_STATEMENT;
}
//---------------------------------------------------------------------------
AssignmentStatementAST::AssignmentStatementAST(management::CodeManager* manager) : StatementAST(manager)
{}
//---------------------------------------------------------------------------
AssignmentStatementAST::AssignmentStatementAST(management::CodeManager* manager, std::unique_ptr<IdentifierAST> left, std::unique_ptr<ExpressionAST> right) : AssignmentStatementAST(manager){
    this->leftIdentifier = move(left) ;
    this->rightExpression = move(right) ;
}
//---------------------------------------------------------------------------
const IdentifierAST& AssignmentStatementAST::getLeftIdentifier() const {
    return *leftIdentifier ;
}
//---------------------------------------------------------------------------
const ExpressionAST& AssignmentStatementAST::getRightExpression() const {
    return *rightExpression ;
}
//---------------------------------------------------------------------------
void AssignmentStatementAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> AssignmentStatementAST::evaluate(EvaluationContext& evaluationContext) const {
    return rightExpression->evaluate(evaluationContext);
}
//---------------------------------------------------------------------------
std::optional<int64_t> AssignmentStatementAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
AssignmentStatementAST::AssignmentStatementAST(std::unique_ptr<IdentifierAST> left, std::unique_ptr<ExpressionAST> right) : leftIdentifier(std::move(left)) , rightExpression(std::move(right)) {}
//---------------------------------------------------------------------------
ASTNode::ASTType ReturnStatementAST::getAstType() const {
    return ASTNode::ASTType::RETURN_STATEMENT;
}
//---------------------------------------------------------------------------
ReturnStatementAST::ReturnStatementAST(management::CodeManager* manager, std::unique_ptr<ExpressionAST> input) : StatementAST(manager) {
    this->input = move(input) ;
}
//---------------------------------------------------------------------------
const ExpressionAST& ReturnStatementAST::getInput() const {
    return *input ;
}
//---------------------------------------------------------------------------
void ReturnStatementAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> ReturnStatementAST::evaluate(EvaluationContext& evaluationContext) const {
    return input->evaluate(evaluationContext);
}
//---------------------------------------------------------------------------
std::optional<int64_t> ReturnStatementAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
ASTNode::ASTType BinaryExpressionAST::getAstType() const {
    return ASTNode::ASTType::BINARY_EXPRESSION;
}
//---------------------------------------------------------------------------
BinaryExpressionAST::BinaryExpressionAST(management::CodeManager* manager, BinaryExpressionAST::BinaryType type, std::unique_ptr<ExpressionAST> left, std::unique_ptr<ExpressionAST> right) : ExpressionAST(manager){
    this->binaryType = type ;
    this->leftExpression = move(left) ;
    this->rightExpression = move(right) ;
}
//---------------------------------------------------------------------------
BinaryExpressionAST::BinaryType BinaryExpressionAST::getBinaryType() const {
    return binaryType ;
}
//---------------------------------------------------------------------------
const ExpressionAST& BinaryExpressionAST::getLeftExpression() const {
    return *leftExpression ;
}
//---------------------------------------------------------------------------
const ExpressionAST& BinaryExpressionAST::getRightExpression() const {
    return *rightExpression ;
}
//---------------------------------------------------------------------------
void BinaryExpressionAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> BinaryExpressionAST::evaluate(EvaluationContext& evaluationContext) const {
    // evaluate left and right expressions
    optional<int64_t> leftResult = leftExpression->evaluate(evaluationContext) ;
    optional<int64_t> rightResult = rightExpression->evaluate(evaluationContext) ;
    if(!leftResult.has_value() || !rightResult.has_value())
        // if runtime error is triggered
        return nullopt ;
    // evaluate current binary expression
    switch (getBinaryType()) {
        case BinaryType::PLUS: return leftResult.value() + rightResult.value();
        case BinaryType::MINUS: return leftResult.value() - rightResult.value();
        case BinaryType::MULTIPLY: return leftResult.value() * rightResult.value();
        case BinaryType::DIVIDE: {
            if(rightResult.value() == 0) {
                // trigger runtime error given position of "/" operator
                codeManager->printDivZeroError(codeReference);
                return nullopt ;
            }
            return leftResult.value() / rightResult.value() ;
        }
    }
    return nullopt;
}
//---------------------------------------------------------------------------
std::optional<int64_t> BinaryExpressionAST::acceptOptimization(OptimizationVisitor& astVisitor) {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
BinaryExpressionAST::BinaryExpressionAST(management::CodeManager* manager, BinaryExpressionAST::BinaryType type, std::unique_ptr<ExpressionAST> left, std::unique_ptr<ExpressionAST> right, management::CodeReference reference) : BinaryExpressionAST(manager , type , move(left) , move(right)){
    codeReference = reference ;
}
//---------------------------------------------------------------------------
ASTNode::ASTType UnaryExpressionAST::getAstType() const {
    return ASTNode::ASTType::UNARY_EXPRESSION;
}
//---------------------------------------------------------------------------
UnaryExpressionAST::UnaryExpressionAST(management::CodeManager* manager, management::CodeReference codeReference1, UnaryExpressionAST::UnaryType type, std::unique_ptr<ExpressionAST> input) : ExpressionAST(manager , codeReference1){
    this->unaryType = type ;
    this->input = move(input);
}
//---------------------------------------------------------------------------
UnaryExpressionAST::UnaryType UnaryExpressionAST::getUnaryType() const {
    return unaryType ;
}
//---------------------------------------------------------------------------
const ExpressionAST& UnaryExpressionAST::getInput() const {
    return *input ;
}
//---------------------------------------------------------------------------
void UnaryExpressionAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> UnaryExpressionAST::evaluate(EvaluationContext& evaluationContext) const {
    optional<int64_t> result = input->evaluate(evaluationContext) ;
    if(!result.has_value())
        return nullopt ;
    if(getUnaryType() == UnaryType::MINUS)
        return -result.value() ;
    return result ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> UnaryExpressionAST::acceptOptimization(OptimizationVisitor& astVisitor)  {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
ASTNode::ASTType IdentifierAST::getAstType() const {
    return ASTNode::ASTType::IDENTIFIER;
}
//---------------------------------------------------------------------------
IdentifierAST::IdentifierAST(management::CodeManager* manager, management::CodeReference codeReference) : ExpressionAST(manager, codeReference)
{}
//---------------------------------------------------------------------------
std::string_view IdentifierAST::print_token() const {
    size_t line  = codeReference.getStartLineRange().first ;
    size_t begin = codeReference.getStartLineRange().second ;
    size_t last = codeReference.getEndLineRange().second ;
    return codeManager->getCurrentLine(line).substr(begin , last - begin + 1) ;
}
//---------------------------------------------------------------------------
void IdentifierAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> IdentifierAST::evaluate(EvaluationContext& evaluationContext) const {
    return evaluationContext.getIdentifier(print_token());
}
//---------------------------------------------------------------------------
std::optional<int64_t> IdentifierAST::acceptOptimization(OptimizationVisitor& astVisitor) {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
ASTNode::ASTType LiteralAST::getAstType() const {
    return ASTNode::ASTType::LITERAL;
}
//---------------------------------------------------------------------------
LiteralAST::LiteralAST(management::CodeManager* manager, management::CodeReference codeReference) : ExpressionAST(manager, codeReference) {
     size_t line  = codeReference.getStartLineRange().first ;
     size_t begin = codeReference.getStartLineRange().second ;
     size_t last = codeReference.getEndLineRange().second ;
     string_view token = codeManager->getCurrentLine(line).substr(begin , last - begin + 1) ;
     this->value = str_to_int64(token) ;
}
//---------------------------------------------------------------------------
void LiteralAST::accept(ASTVisitor& astVisitor) const {
    astVisitor.visit(*this) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> LiteralAST::evaluate(EvaluationContext& /*evaluationContext*/) const {
    return value ;
}
//---------------------------------------------------------------------------
int64_t LiteralAST::getValue() const {
    return value ;
}
//---------------------------------------------------------------------------
LiteralAST::LiteralAST(int64_t value) : value(value){
}
//---------------------------------------------------------------------------
std::optional<int64_t> LiteralAST::acceptOptimization(OptimizationVisitor& astVisitor) {
    return astVisitor.visitOptimization(*this) ;
}
//---------------------------------------------------------------------------
size_t ASTNode::getNodeID() const{
    return node_index;
}
//---------------------------------------------------------------------------
SymbolTable& ASTNode::getSymbolTable()  {
    return symbolTable ;
}
std::string ASTNode::visualizeDot() const {
    semantic::VisualizeASTVisitor printVisitor ;
    this->accept(printVisitor) ;
    return printVisitor.getOutput() ;
}
std::string ASTNode::testDot() const {
    semantic::TestPrintASTVisitor printVisitor ;
    this->accept(printVisitor) ;
    return printVisitor.getOutput() ;
}
//---------------------------------------------------------------------------
ASTNode::~ASTNode() = default ;
//---------------------------------------------------------------------------
StatementAST::StatementAST() {
    node_index = node_index_incrementer++ ;
}
//---------------------------------------------------------------------------
StatementAST::StatementAST(management::CodeManager* manager) : StatementAST(){
    this->codeManager = manager ;
}
//---------------------------------------------------------------------------
StatementAST::StatementAST(management::CodeManager* manager, management::CodeReference codeReference1) : StatementAST(manager) {
    this->codeReference = codeReference1 ;
}
//---------------------------------------------------------------------------
ExpressionAST::ExpressionAST() {
    node_index = node_index_incrementer++ ;
}
//---------------------------------------------------------------------------
ExpressionAST::ExpressionAST(management::CodeManager* manager):ExpressionAST() {
    this->codeManager = manager ;
}
//---------------------------------------------------------------------------
ExpressionAST::ExpressionAST(management::CodeManager* manager, management::CodeReference codeReference1) : ExpressionAST(manager){
    this->codeReference = codeReference1 ;
}
//---------------------------------------------------------------------------
} //namespace jitcompiler::semantic
//---------------------------------------------------------------------------