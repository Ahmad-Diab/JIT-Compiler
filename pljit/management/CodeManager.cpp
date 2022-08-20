#include "CodeManager.hpp"
#include <cassert>
#include <iostream>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler ::management{

CodeManager::CodeManager(string_view sourceCode) {
    for(size_t i = 0 , j = 0 , code_size = sourceCode.size() ;i < code_size && j < code_size ; i++ , j++) {
        while (j < code_size && sourceCode[j] != '\n')
            j++ ;
        code_lines.emplace_back(sourceCode.substr(i , j - i)) ;
        i = j ;
    }
}
std::string_view CodeManager::getCurrentLine(size_t index) const {
    return code_lines[index] ;
}
void CodeManager::printTokenFailure(CodeReference codeReference) {
    assert(codeReference.getStartLineRange().first == codeReference.getEndLineRange().first) ;
    size_t currentLine = codeReference.getStartLineRange().first ;
    size_t start_index = codeReference.getStartLineRange().second ;
    size_t last_index = codeReference.getEndLineRange().second ;
    assert(currentLine < code_lines.size()) ;
    errorOccurred = true ;
    string_view invalidToken = code_lines[currentLine].substr(start_index , last_index - start_index + 1) ;
    compileErrorStream << currentLine + 1 << ":" << start_index + 1 << ": error: unexpected token \"" << invalidToken << "\"\n" ;
    compileErrorStream << code_lines[currentLine] << '\n' ;
    compileErrorStream.width(static_cast<uint32_t>(start_index + 1)) ;
    compileErrorStream << '^' ;
    while (start_index < last_index) {
        compileErrorStream << "~" ;
        start_index ++ ;
    }
    compileErrorStream << '\n' ;
}
void CodeManager::printCompileError(CodeReference codeReference , std::string_view expectedToken) {
    assert(codeReference.getStartLineRange().first == codeReference.getEndLineRange().first) ;
    size_t currentLine = codeReference.getStartLineRange().first ;
    size_t start_index = codeReference.getStartLineRange().second ;
    size_t last_index = codeReference.getEndLineRange().second ;
    assert(currentLine < code_lines.size()) ;
    errorOccurred = true ;
    compileErrorStream << currentLine + 1 << ":" << start_index + 1 << ": error: ";
    if(!expectedToken.empty())
        compileErrorStream << "expected " << expectedToken << '\n' ;
    else
        compileErrorStream << '\n' ;

    compileErrorStream << code_lines[currentLine] << '\n' ;
    compileErrorStream.width(static_cast<uint32_t>(start_index + 1)) ;
    compileErrorStream << '^' ;
    while (start_index < last_index) {
        compileErrorStream << "~" ;
        start_index ++ ;
    }
    compileErrorStream << '\n' ;
}
void CodeManager::printDivZeroError(CodeReference codeReference) {
    assert(codeReference.getStartLineRange().first == codeReference.getEndLineRange().first) ;
    size_t currentLine = codeReference.getStartLineRange().first ;
    size_t start_index = codeReference.getStartLineRange().second ;
    size_t last_index = codeReference.getEndLineRange().second ;
    assert(currentLine < code_lines.size()) ;
    errorOccurred = true ;
    runtimeErrorStream << currentLine + 1 << ":" << start_index + 1 << ": Runtime Error: ";
    runtimeErrorStream << "Divide by Zero" << '\n';

    runtimeErrorStream << code_lines[currentLine] << '\n' ;
    runtimeErrorStream.width(static_cast<uint32_t>(start_index + 1)) ;
    runtimeErrorStream << '^' ;
    while (start_index < last_index) {
        runtimeErrorStream << "~" ;
        start_index ++ ;
    }
    runtimeErrorStream << '\n' ;
}

void CodeManager::printSemanticError(CodeReference codeReference, std::string_view message) {
    assert(codeReference.getStartLineRange().first == codeReference.getEndLineRange().first) ;
    size_t currentLine = codeReference.getStartLineRange().first ;
    size_t start_index = codeReference.getStartLineRange().second ;
    size_t last_index = codeReference.getEndLineRange().second ;
    assert(currentLine < code_lines.size()) ;
    errorOccurred = true ;
    compileErrorStream << currentLine + 1 << ":" << start_index + 1 << ": error: ";
    if(!message.empty())
        compileErrorStream << message << '\n';
    else
        compileErrorStream << '\n' ;
    compileErrorStream << code_lines[currentLine] << '\n' ;
    compileErrorStream.width(static_cast<uint32_t>(start_index + 1)) ;
    compileErrorStream << '^' ;
    while (start_index < last_index) {
        compileErrorStream << "~" ;
        start_index ++ ;
    }
    compileErrorStream << '\n' ;
}

void CodeManager::printCompileError(size_t token_length , std::string_view expectedToken ) {

    errorOccurred = true ;
    if(code_lines.empty())
        compileErrorStream << "1:1" << ": error: ";
    else {
        size_t currentLine = code_lines.size() - 1;
        assert(!code_lines[currentLine].empty()) ; // assume no empty line
        size_t start_index = code_lines[currentLine].size() ;

        compileErrorStream << currentLine + 1 << ":" << start_index + 1 << ": error: ";
    }
    if(!expectedToken.empty())
        compileErrorStream << "expected \"" << expectedToken << "\"\n" ;
    else
        compileErrorStream << '\n' ;
    if(!code_lines.empty()) {
        size_t currentLine = code_lines.size() - 1;
        compileErrorStream << code_lines[currentLine] << '\n' ;
        assert(!code_lines[currentLine].empty()) ; // assume no empty line
        size_t start_index = code_lines[currentLine].size() ;
        compileErrorStream.width(static_cast<uint32_t>(start_index + 1)) ;
        compileErrorStream << '^' ;
        while (token_length > 1) {
            compileErrorStream << "~" ;
            token_length -- ;
        }
    }
    else {
        compileErrorStream << "\n^"  ;
        while (token_length > 1) {
            compileErrorStream << "~" ;
            token_length -- ;
        }
    }
}

std::size_t CodeManager::countLines() const {
    return code_lines.size() ;
}
bool CodeManager::isCodeError() const{
    return errorOccurred ;
}
std::string CodeManager::error_message() const {
    return compileErrorStream.str() ;
}
std::string CodeManager::runtimeErrorMessage() {
    string res = runtimeErrorStream.str() ;
    runtimeErrorStream.str("");
    return res ;
}

} // namespace jitcompiler::management
//---------------------------------------------------------------------------
