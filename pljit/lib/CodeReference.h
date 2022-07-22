#ifndef PLJIT_CODEREFERENCE_H
#define PLJIT_CODEREFERENCE_H
//---------------------------------------------------------------------------
#include <cstddef>
#include <utility>
namespace jitcompiler {
//---------------------------------------------------------------------------
class CodeReference{
    private:
    std::pair<std::size_t , std::size_t> line_range ;
    std::pair<std::size_t , std::size_t> start_line_range ;
    std::pair<std::size_t , std::size_t> end_line_range ; // end is inclusive
    public:
    CodeReference()  ;
    CodeReference(std::pair<std::size_t , std::size_t> line_range , std::pair<std::size_t , std::size_t> start_line_range , std::pair<std::size_t , std::size_t> end_line_range) ;

    std::pair<std::size_t , std::size_t > getLineRange() const;
    std::pair<std::size_t , std::size_t > getStartLineRange() const;
    std::pair<std::size_t , std::size_t > getEndLineRange() const;

};

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------

#endif //PLJIT_CODEREFERENCE_H
