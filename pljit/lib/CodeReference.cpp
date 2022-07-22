#include "pljit/lib/CodeReference.h"

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

CodeReference::CodeReference(std::pair<std::size_t, std::size_t> line_range, std::pair<std::size_t, std::size_t> start_line_range, std::pair<std::size_t, std::size_t> end_line_range) :line_range(line_range) , start_line_range(start_line_range) , end_line_range(end_line_range){

}
std::pair<std::size_t, std::size_t> CodeReference::getLineRange() const {
    return line_range ;
}
std::pair<std::size_t, std::size_t> CodeReference::getStartLineRange() const {
    return start_line_range ;
}
std::pair<std::size_t, std::size_t> CodeReference::getEndLineRange() const {
    return end_line_range ;
}
CodeReference::CodeReference() {}

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
