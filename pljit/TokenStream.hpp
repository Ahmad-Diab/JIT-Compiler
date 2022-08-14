#ifndef PLJIT_TOKENSTREAM_HPP
#define PLJIT_TOKENSTREAM_HPP
//---------------------------------------------------------------------------
#include "CodeManager.hpp"
namespace jitcompiler {
//---------------------------------------------------------------------------

class TokenStream {
public:
    enum TokenType {
        KEYWORD,
        IDENTIFIER,
        LITERAL,
        COMMA_SEPARATOR,
        SEMI_COLON_SEPARATOR,
        TERMINATOR,
        CONST_ASSIGNMENT,
        VAR_ASSIGNMENT,
        PLUS_OPERATOR,
        MINUS_OPERATOR,
        MULTIPLY_OPERATOR,
        DIVIDE_OPERATOR,
        OPEN_BRACKET,
        CLOSE_BRACKET
    };
    class Token {
        // TODO Replace with codeReference
        CodeReference codeReference ;
        TokenType type ;

        public:
        explicit Token(CodeReference reference , TokenType tokenType) : codeReference(std::move(reference)) , type(tokenType){}
        CodeReference& getCodeReference()  ;
        TokenType getTokenType() const ;
    };

    explicit TokenStream(CodeManager* currentManager) ;

    Token lookup() const ; // rename lookup

    Token nextToken() ; // nextToken -> return nextToken

    bool isEmpty() const ;

    void compileCode()  ;

    void reset() ;

private:
    CodeManager *manager ;
    std::vector<Token> streamTokens ;
    size_t iterator_token = 0 ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_TOKENSTREAM_HPP
