#include "CodeReference.h"

//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

CodeReference::CodeReference(std::pair<size_t, size_t> line_range, std::pair<size_t, size_t> start_line_range, std::pair<size_t,size_t> end_line_range) :line_range(std::move(line_range)) , start_line_range(std::move(start_line_range)) , end_line_range(std::move(end_line_range)){

}
std::pair<size_t, size_t> CodeReference::getLineRange() const {
    return line_range ;
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
