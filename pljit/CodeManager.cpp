#include "CodeManager.h"
#include <cassert>
#include <iostream>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler{

CodeManager::CodeManager(string_view sourceCode) {
    for(size_t i = 0 , j = 0 , code_size = sourceCode.size() ;i < code_size && j < code_size ; i++ , j++) {
        while (j < code_size && sourceCode[j] != '\n')
            j++ ;
        lines.emplace_back(sourceCode.substr(i , j - i)) ;
        i = j ;
    }
}
std::string_view CodeManager::getCurrentLine(size_t index) const {
    return lines[index] ;
}

void CodeManager::printCompileError(size_t currentLine, size_t start_index, size_t last_index , std::string_view expectedToken) {
    assert(currentLine < lines.size()) ;
    errorOccurred = true ;
    errStream << currentLine << ":" << start_index << ": error: ";
    if(!expectedToken.empty())
        errStream << "expected " << expectedToken << '\n' ;
    else
        errStream << '\n' ;

    errStream << lines[currentLine] << '\n' ;
    errStream.width(static_cast<uint32_t>(start_index + 1)) ;
    errStream << '^' ;
    while (start_index < last_index) {
        errStream << "~" ;
        start_index ++ ;
    }
}
void CodeManager::printCompileError(size_t token_length , std::string_view expectedToken ) {

    errorOccurred = true ;
    if(lines.empty())
    {

    }
    else {
        size_t currentLine = lines.size() - 1;
        assert(lines[currentLine].size()) ; // assume no empty line
        size_t start_index = lines[currentLine].size() ;

        errStream << currentLine << ":" << start_index << ": error: ";
    }
    if(!expectedToken.empty())
        errStream << "expected \"" << expectedToken << "\"\n" ;
    else
        errStream << '\n' ;
    if(!lines.empty()) {
        size_t currentLine = lines.size() - 1;
        errStream << lines[currentLine] << '\n' ;
        assert(lines[currentLine].size()) ; // assume no empty line
        size_t start_index = lines[currentLine].size() ;
        errStream.width(static_cast<uint32_t>(start_index + 1)) ;
        errStream << '^' ;
        while (token_length > 1) {
            errStream << "~" ;
            token_length -- ;
        }

    }
}

std::size_t CodeManager::countLines() const {
    return lines.size() ;
}
bool CodeManager::isCodeError() const{
    return errorOccurred ;
}
std::string CodeManager::error_message() const {
    return errStream.str() ;
}

} // namespace jitcompiler
//---------------------------------------------------------------------------
