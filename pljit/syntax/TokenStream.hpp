#ifndef PLJIT_TOKENSTREAM_HPP
#define PLJIT_TOKENSTREAM_HPP
//---------------------------------------------------------------------------
#include "pljit/management/CodeManager.hpp"
namespace jitcompiler ::syntax{
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
        management::CodeReference codeReference ;
        TokenType type ;

        public:
        explicit Token(management::CodeReference reference , TokenType tokenType) : codeReference(std::move(reference)) , type(tokenType){}
        management::CodeReference& getCodeReference()  ;
        TokenType getTokenType() const ;
    };

    explicit TokenStream(management::CodeManager* currentManager) ;

    Token lookup() const ; // rename lookup

    Token nextToken() ; // nextToken -> return nextToken

    bool isEmpty() const ;

    void compileCode()  ;

private:
    management::CodeManager *manager ;
    std::vector<Token> streamTokens ;
    size_t iterator_token = 0 ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------
#endif //PLJIT_TOKENSTREAM_HPP
