#ifndef PLJIT_CODEREFERENCE_HPP
#define PLJIT_CODEREFERENCE_HPP
//---------------------------------------------------------------------------
#include <cstddef>
#include <utility>
namespace jitcompiler {
//---------------------------------------------------------------------------
class CodeReference{
    private:
    std::pair<std::size_t , std::size_t> start_line_range ; // first-> line , second-> start_index
    std::pair<std::size_t , std::size_t> end_line_range ; // end is inclusive & first->line , second -> end_index(inclusive)

    public:
    CodeReference()  ;
    CodeReference(std::pair<std::size_t , std::size_t> start_line_range , std::pair<std::size_t , std::size_t> end_line_range) ;

    std::pair<std::size_t , std::size_t > getStartLineRange() const ;
    std::pair<std::size_t , std::size_t > getEndLineRange() const ;
};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_CODEREFERENCE_HPP
