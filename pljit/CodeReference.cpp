#include "CodeReference.hpp"

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

CodeReference::CodeReference(std::pair<size_t, size_t> start_line_range, std::pair<size_t,size_t> end_line_range) :start_line_range(std::move(start_line_range)) , end_line_range(std::move(end_line_range)) {
}
std::pair<size_t, size_t> CodeReference::getStartLineRange() const {
    return start_line_range ;
}
std::pair<size_t, size_t> CodeReference::getEndLineRange() const {
    return end_line_range ;
}
CodeReference::CodeReference()  = default ;

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
