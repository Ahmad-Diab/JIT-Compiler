#include "pljit/EvaluationContext.h"
#include "pljit/AST.h"
#include <cassert>

using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler {
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
EvaluationContext::EvaluationContext(const std::vector<int64_t>& parameterList, const SymbolTable& symbolTable) {
    for(auto &[identifier , e] : symbolTable.getTableContent())
    {
        SymbolTable::AttributeType type = get<0>(e) ;
        size_t index = get<2>(e) ;
        std::optional<int64_t > cur_value = get<3>(e) ;
        if(type == SymbolTable::AttributeType::PARAMETER)
        {
            assert(!cur_value) ;
            assert(index < parameterList.size()) ;
            int64_t parameter_value = parameterList[index] ;
            pushParameter(identifier) ;
            updateIdentifier(identifier , parameter_value) ;
        }
        else if(type == SymbolTable::AttributeType::VARIABLE)
        {
            assert(!cur_value) ;
            variables[identifier] ;
        }
        else {
            assert(cur_value) ;
            pushConstant(identifier , cur_value.value()) ;
        }
    }
}
EvaluationContext::EvaluationContext(const SymbolTable& symbolTable) {
    for(auto &[identifier , e] : symbolTable.getTableContent())
    {
        SymbolTable::AttributeType type = get<0>(e) ;
        std::optional<int64_t > cur_value = get<3>(e) ;
        if(type == SymbolTable::AttributeType::PARAMETER)
        {
            assert(!cur_value) ;
            pushParameter(identifier) ;
        }
        else if(type == SymbolTable::AttributeType::VARIABLE)
        {
            assert(!cur_value) ;
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
