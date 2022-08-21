#ifndef PLJIT_CODEREFERENCE_HPP
#define PLJIT_CODEREFERENCE_HPP
//---------------------------------------------------------------------------
#include <cstddef>
#include <utility>
//---------------------------------------------------------------------------
namespace jitcompiler::management {
//---------------------------------------------------------------------------
class CodeReference{
    private:
    // .first-> line , .second-> start_index(inclusive)
    std::pair<std::size_t , std::size_t> start_line_range ;
    // .first->line , .second -> end_index(inclusive)
    std::pair<std::size_t , std::size_t> end_line_range ;

    public:
    // default constructor
    CodeReference()  ;
    // Constructor(start , end)
    CodeReference(std::pair<std::size_t , std::size_t> start_line_range , std::pair<std::size_t , std::size_t> end_line_range) ;
    // get position of first character
    std::pair<std::size_t , std::size_t > getStartLineRange() const ;
    // get position of last character
    std::pair<std::size_t , std::size_t > getEndLineRange() const ;
};
//---------------------------------------------------------------------------
} // namespace jitcompiler::management
//---------------------------------------------------------------------------
#endif //PLJIT_CODEREFERENCE_HPP
