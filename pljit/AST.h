#ifndef PLJIT_AST_H
#define PLJIT_AST_H
//---------------------------------------------------------------------------
namespace jitcompiler {
//---------------------------------------------------------------------------

class ASTNode {
    protected:

    public:
    enum class Type{
        FUNCTION,
        RETURN_STATEMENT,
        ASSIGNMENT_STATEMENT,
        BINARY_EXPRESSION,
        UNARY_EXPRESSION,
        IDENTIFIER,
        LITERAL
    };
};



//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_AST_H
