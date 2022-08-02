#include "CodeManager.h"
#include <cassert>
#include <iostream>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace jitcompiler{

CodeManager::CodeManager(string_view sourceCode) {
    for(size_t i = 0 , j = 0 , code_size = sourceCode.size() ;i < code_size && j < code_size ; i++ , j++)
    {
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

    cout << currentLine << ":" << start_index << ": error: ";
    if(!expectedToken.empty())
        cout << "expected " << expectedToken << '\n' ;
    else
        cout << '\n' ;
    cout << lines[currentLine] << '\n' ;
    cout.width(static_cast<uint32_t>(start_index + 1)) ;
    cout << '^' ;
    while (start_index < last_index) {
        cout << "~" ;
        start_index ++ ;
    }
}
std::size_t CodeManager::countLines() const {
    return lines.size() ;
}

} // namespace jitcompiler
//---------------------------------------------------------------------------
