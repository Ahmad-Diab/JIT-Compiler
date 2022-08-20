#ifndef PLJIT_CODEMANAGER_HPP
#define PLJIT_CODEMANAGER_HPP
//---------------------------------------------------------------------------
#include "CodeReference.hpp"
#include <sstream>
#include <string_view>
#include <vector>
namespace jitcompiler ::management{
//---------------------------------------------------------------------------
class CodeManager
{
    private:
    std::vector<std::string_view> code_lines ;
    std :: ostringstream compileErrorStream ;
    std :: ostringstream runtimeErrorStream ;
    bool errorOccurred = false;

    public:
    explicit CodeManager(std::string_view sourceCode) ;

    std::string_view getCurrentLine(size_t index) const ;

    std::size_t countLines() const ;

    void printTokenFailure(CodeReference codeReference) ;

    void printCompileError(CodeReference codeReference, std::string_view expectedToken = "") ;

    void printCompileError(size_t token_length , std::string_view expectedToken = "") ;

    void printSemanticError(CodeReference codeReference , std::string_view message = "") ;

    void printDivZeroError(CodeReference codeReference) ;

    bool isCodeError() const ;

    std::string error_message() const ;

    std::string runtimeErrorMessage();
};
//---------------------------------------------------------------------------
} //namespace jitcompiler::management
//---------------------------------------------------------------------------
#endif //PLJIT_CODEMANAGER_HPP