#ifndef PLJIT_CODEMANAGER_H
#define PLJIT_CODEMANAGER_H
//---------------------------------------------------------------------------
#include <string_view>
#include <vector>
namespace jitcompiler{
//---------------------------------------------------------------------------
class CodeManager
{
    private:
    std::vector<std::string_view> lines ;

    public:
    explicit CodeManager(std::string_view sourceCode) ;

    std::string_view getCurrentLine(size_t index) const ;

    std::size_t countLines() const ;

    void printCompileError(size_t currentLine , size_t start_index , size_t last_index) ;

};
//---------------------------------------------------------------------------
} //namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_CODEMANAGER_H