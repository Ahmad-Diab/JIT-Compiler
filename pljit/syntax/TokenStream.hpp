#ifndef PLJIT_TOKENSTREAM_HPP
#define PLJIT_TOKENSTREAM_HPP
//---------------------------------------------------------------------------
#include "pljit/management/CodeManager.hpp"
//---------------------------------------------------------------------------
namespace jitcompiler ::syntax{
//---------------------------------------------------------------------------
class TokenStream {
public:
    //---------------------------------------------------------------------------
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
    //---------------------------------------------------------------------------
    class Token {
        private:
        /// reference of token in source code (can be retrieved from CodeManager)
        management::CodeReference codeReference ;
        /// token type
        TokenType type ;
        public:
        /// Token Constructor with codeRef and token type
        explicit Token(management::CodeReference reference , TokenType tokenType) ;
        /// get CodeReference from member variable
        management::CodeReference getCodeReference()  ;
        /// get TokenType from member variable
        TokenType getTokenType() const ;
    };
    //---------------------------------------------------------------------------
    /// Constructor for TokenStream (without code compilation)
    explicit TokenStream(management::CodeManager* currentManager) ;
    //---------------------------------------------------------------------------
    /// Compile code after construction and check if compilation process succeed
    bool compileCode()  ;
    //---------------------------------------------------------------------------
    /// After compilation , check if token stream is empty
    bool isEmpty() const ;
    //---------------------------------------------------------------------------
    /// After compilation , check for a token on front stream
    Token lookup() const ;
    //---------------------------------------------------------------------------
    /// After compilation , return token and remove it from front stream
    Token nextToken() ;
    //---------------------------------------------------------------------------
private:
    /// CodeManager for source code
    management::CodeManager *manager ;
    /// Stream of tokens after calling member function -> compileCode()
    std::vector<Token> streamTokens ;
    /// iterator for member function -> nextToken()
    size_t iterator_token = 0 ;
};
//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------
#endif //PLJIT_TOKENSTREAM_HPP
