#include "EvaluationContext.hpp"
#include "AST.hpp"
#include <cassert>
#include <iostream>

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
void EvaluationContext::pushVariable(std::string_view identifier) {
    assert(parameters.find(identifier) == parameters.end()) ;
    assert(variables.find(identifier) == variables.end()) ;
    assert(constants.find(identifier) == constants.end()) ;
    parameters[identifier] = nullopt ;
}
void EvaluationContext::pushConstant(std::string_view identifier, int64_t value) {
    assert(parameters.find(identifier) == parameters.end()) ;
    assert(variables.find(identifier) == variables.end()) ;
    assert(constants.find(identifier) == constants.end()) ;
    constants[identifier] = value ;
}
void EvaluationContext::updateIdentifier(std::string_view identifier, int64_t value) {
    assert(constants.find(identifier) == constants.end()) ;
    if(parameters.find(identifier) != parameters.end())
        parameters[identifier] = value ;
    else if(variables.find(identifier) != variables.end())
        variables[identifier] = value ;
    else
        assert(false) ;
}
std::optional<int64_t> EvaluationContext::getIdentifier(std::string_view identifier) {
    if(parameters.find(identifier) != parameters.end())
        return parameters[identifier] ;
    if(variables.find(identifier) != variables.end())
        return variables[identifier] ;
    if(constants.find(identifier) != constants.end())
        return constants[identifier] ;
    return std::nullopt;
}
EvaluationContext::EvaluationContext(std::vector<int64_t>& parameterList, SymbolTable& symbolTable) :symbolTable(&symbolTable){
    constexpr size_t PARAMETER = 0 , VARIABLE = 1 , CONSTANT = 2 ;
    for(size_t type = 0 ; type < 3 ; ++type)
        for(auto &[identifier , e] : symbolTable.getTableContent()[type])
        {
            size_t index = get<1>(e) ;
            std::optional<int64_t > cur_value = get<2>(e) ;
            if(type == PARAMETER)
            {
                assert(index < parameterList.size()) ;
                int64_t parameter_value = parameterList[index] ;
                pushParameter(identifier) ;
                updateIdentifier(identifier , parameter_value) ;
            }
            else if(type == VARIABLE)
            {
                assert(!cur_value) ;
                variables[identifier] ;
            }
            else if(type == CONSTANT) {
                assert(cur_value) ;
                pushConstant(identifier , cur_value.value()) ;
            }
        }
}
EvaluationContext::EvaluationContext(SymbolTable& symbolTable) {
    this->symbolTable = &symbolTable ;
    for(size_t type = 0 ; type < 3 ; ++type)
        for(auto &[identifier , e] : symbolTable.getTableContent()[type]){
            std::optional<int64_t > cur_value = get<2>(e) ;
            if(type == SymbolTable::AttributeType::PARAMETER) {
                pushParameter(identifier) ;
            }
            else if(type == SymbolTable::AttributeType::VARIABLE) {
                pushVariable(identifier) ;
            }
            else {
                assert(cur_value) ;
                pushConstant(identifier , cur_value.value()) ;
            }

        }
}

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
