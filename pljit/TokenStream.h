#ifndef PLJIT_TOKENSTREAM_H
#define PLJIT_TOKENSTREAM_H
//---------------------------------------------------------------------------
#include "CodeManager.h"
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
        // TODO check member variables
        public:
        size_t line , start_index , last_index ;
        TokenType type ;
        explicit Token(size_t line , size_t start_index , size_t last_index , TokenType tokenType) : line(line) , start_index(start_index) , last_index(last_index) , type(tokenType){}

    };

    explicit TokenStream(CodeManager* currentManager) ;

    Token getNextToken() const ;

    void popNextToken() ;

    bool isEmpty() const ;

    // TODO remove method ;
    void reset() {
        iterator_token = 0 ;
    }
private:
    CodeManager *manager ;
    std::vector<Token> streamTokens ;
    size_t iterator_token = 0 ;

};
//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
#endif //PLJIT_TOKENSTREAM_H
