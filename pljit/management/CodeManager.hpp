#ifndef PLJIT_CODEMANAGER_HPP
#define PLJIT_CODEMANAGER_HPP
//---------------------------------------------------------------------------
#include "pljit/management/CodeReference.hpp"
//---------------------------------------------------------------------------
#include <sstream>
#include <string_view>
#include <vector>
namespace jitcompiler ::management{
//---------------------------------------------------------------------------
class CodeManager
{
    private:
    // flag to check if there is any compile error occurred
    bool compileErrorTriggered = false ;
    // store each line of source code
    std::vector<std::string_view> code_lines ;
    // output stream for printing compile error message
    std :: ostringstream compileErrorStream ;
    // output stream for printing runtime error message (divide by zero)
    std :: ostringstream runtimeErrorStream ;

    public:
    // Constructor -> store source code line by line
    explicit CodeManager(std::string_view sourceCode) ;

    // get current line of code -> zero-based index
    std::string_view getCurrentLine(size_t index) const ;

    // count # lines of source code
    std::size_t countLines() const ;

    // trigger unexpected token
    void printTokenFailure(CodeReference codeReference) ;

    // trigger unexpected token and print expected token
    void printCompileError(CodeReference codeReference, std::string_view expectedToken) ;

    // trigger EOF and print expected token
    void printCompileError(size_t expected_token_length , std::string_view expectedToken) ;

    // trigger semantic compile error
    void printSemanticError(CodeReference codeReference , std::string_view message) ;

    // trigger runtime error -> divide by zero
    void printDivZeroError(CodeReference codeReference) ;

    // check if compile error is triggered
    bool isCodeError() const ;

    // print compile error message
    std::string error_message() const ;

    // print compile error message
    std::string runtimeErrorMessage();
};
//---------------------------------------------------------------------------
} //namespace jitcompiler::management
//---------------------------------------------------------------------------
#endif //PLJIT_CODEMANAGER_HPP