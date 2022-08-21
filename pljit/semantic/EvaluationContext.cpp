#include "pljit/semantic/EvaluationContext.hpp"
#include "pljit/semantic/AST.hpp"
//---------------------------------------------------------------------------
#include <cassert>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler ::semantic{
//---------------------------------------------------------------------------
void EvaluationContext::pushParameter(std::string_view identifier) {
    assert(parameters.find(identifier) == parameters.end()) ;
    assert(variables.find(identifier) == variables.end()) ;
    assert(constants.find(identifier) == constants.end()) ;
    parameters[identifier] = nullopt ;
}
//---------------------------------------------------------------------------
void EvaluationContext::pushVariable(std::string_view identifier) {
    assert(parameters.find(identifier) == parameters.end()) ;
    assert(variables.find(identifier) == variables.end()) ;
    assert(constants.find(identifier) == constants.end()) ;
    variables[identifier] = nullopt ;
}
//---------------------------------------------------------------------------
void EvaluationContext::pushConstant(std::string_view identifier, int64_t value) {
    assert(parameters.find(identifier) == parameters.end()) ;
    assert(variables.find(identifier) == variables.end()) ;
    assert(constants.find(identifier) == constants.end()) ;
    constants[identifier] = value ;
}
//---------------------------------------------------------------------------
void EvaluationContext::updateIdentifier(std::string_view identifier, int64_t value) {
    assert(constants.find(identifier) == constants.end()) ;
    if(parameters.find(identifier) != parameters.end())
        parameters[identifier] = value ;
    else if(variables.find(identifier) != variables.end())
        variables[identifier] = value ;
    else
        assert(false) ;
}
//---------------------------------------------------------------------------
std::optional<int64_t> EvaluationContext::getIdentifier(std::string_view identifier) {
    if(parameters.find(identifier) != parameters.end())
        return parameters[identifier] ;
    if(variables.find(identifier) != variables.end())
        return variables[identifier] ;
    if(constants.find(identifier) != constants.end())
        return constants[identifier] ;
    return std::nullopt;
}
//---------------------------------------------------------------------------
EvaluationContext::EvaluationContext(std::vector<int64_t> parameterList, SymbolTable& symbolTable){
    constexpr size_t PARAMETER = 0 , VARIABLE = 1 , CONSTANT = 2 ;
    for(size_t type = 0 ; type < 3 ; ++type)
        for(auto &[identifier , metadata] : symbolTable.getTableContent()[type]) {
            size_t index = get<1>(metadata) ;
            std::optional<int64_t > cur_value = get<2>(metadata) ;
            if(type == PARAMETER) {
                assert(index < parameterList.size()) ;
                pushParameter(identifier) ;

                updateIdentifier(identifier , parameterList[index]) ;
            }
            else if(type == VARIABLE) {
                assert(!cur_value) ;
                pushVariable(identifier);
            }
            else if(type == CONSTANT) {
                assert(cur_value) ;
                pushConstant(identifier , cur_value.value()) ;
            }
        }
}
//---------------------------------------------------------------------------
EvaluationContext::EvaluationContext(SymbolTable& symbolTable) {
    for(size_t type = 0 ; type < 3 ; ++type)
        for(auto &[identifier , metadata] : symbolTable.getTableContent()[type]){
            std::optional<int64_t > cur_value = get<2>(metadata) ;
            if(type == SymbolTable::AttributeType::PARAMETER)
                pushParameter(identifier) ;
            else if(type == SymbolTable::AttributeType::VARIABLE)
                pushVariable(identifier) ;
            else {
                assert(cur_value) ;
                pushConstant(identifier , cur_value.value()) ;
            }

        }
}
//---------------------------------------------------------------------------
} // namespace jitcompiler::semantic
//---------------------------------------------------------------------------
