#include "pljit/syntax/TokenStream.hpp"
#include <array>
#include <cassert>
#include <algorithm>
#include <optional>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace
// helper functions
{
    //---------------------------------------------------------------------------
    constexpr array<string_view , 6> keywords = {"PARAM" , "VAR" , "CONST" , "BEGIN" , "END" , "RETURN"} ;
    //---------------------------------------------------------------------------
    bool isWhiteSpace(char c)
    /// if char is white space
    {
        return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ;
    }
    //---------------------------------------------------------------------------
    bool isDigit(char c)
    /// check if char is digit
    {
        return '0' <= c && c <= '9' ;
    }
    //---------------------------------------------------------------------------
    bool isLetter(char c)
    /// check if char is letter
    {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'z') ;
    }
    //---------------------------------------------------------------------------
    bool isValidSpecialChar(char c)
    /// check if character is valid character for non-letter or non-digit chars
    {
        return c == '.' || c == ',' || c == ';' || c == ':' || c == '+' || c == '-' || c == '/' || c == '*' ||
                c == '=' || c == '(' || c == ')' ;
    }
    //---------------------------------------------------------------------------
    bool isKeyword(string_view token)
    /// check if token is keyword
    {
        return ranges::any_of(keywords.begin() , keywords.end() , [&token](string_view curr){
            return curr == token ;
        }) ;
    }
    //---------------------------------------------------------------------------
    bool isIdentifier(string_view token)
    /// check if token is identifier
    {
        return ranges::all_of(token.begin() , token.end() , [](char c){
            return isLetter(c) ;
        }) ;
    }
    //---------------------------------------------------------------------------
    bool isLiteral(string_view token)
    /// check if token is literal
    {
        return ranges::all_of(token.begin() , token.end() , [](char c){
            return isDigit(c) ;
        }) ;
    }
    //---------------------------------------------------------------------------
    optional<size_t> getNextIndex(string_view currentLine , size_t begin_index , size_t end_index)
    // get end_index (exclusive) which represent valid token [begin_index , next_index)
    {
        size_t next_index = begin_index ;
        // if first character is letter or digit , then move next_index until it hits non letter and non digit char
        if(isLetter(currentLine[begin_index]) || isDigit(currentLine[begin_index])) {
            ++next_index ;
            while(next_index != end_index && (isLetter(currentLine[next_index]) || isDigit(currentLine[next_index])))
                ++next_index ;
            return next_index ;
        }
        // if first character is valid special character then check for valid token of size = 2 ":=" or any valid token of size 1
        // token":=" is valid. However, ": =" and ":\n=" are invalid tokens

        else if(isValidSpecialChar(currentLine[begin_index])) {
            if(currentLine[begin_index] != ':')
                return next_index + 1 ;
            if(begin_index + 1 < end_index && currentLine[begin_index] == ':' && currentLine[begin_index + 1] == '=')
                return next_index + 2 ;
        }
        return std::nullopt ;
    }
    //---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
namespace jitcompiler ::syntax{
//---------------------------------------------------------------------------
TokenStream::TokenStream(management::CodeManager* currentManager) : manager(currentManager)  {}
//---------------------------------------------------------------------------
bool TokenStream::compileCode() {
    for(size_t line_index = 0 ; line_index < manager->countLines() ; line_index++) {
        string_view currentLine = manager->getCurrentLine(line_index) ;

        /// for each line
        for(size_t begin_index = 0 , line_size = currentLine.size() ; begin_index < line_size ; ++begin_index) {
            size_t end_index = begin_index ;
            /// move end_index until it reaches whitespace
            while (end_index < line_size && !isWhiteSpace(currentLine[end_index]))
                ++end_index ;

            while (begin_index != end_index) {
                /// get end_index (exclusive) which represent valid token [begin_index , next_index)
                std::optional<size_t> next_index = getNextIndex(currentLine , begin_index , end_index) ;

                /// if there is valid token occurred
                if(next_index) {
                    size_t current_index = next_index.value() ;

                    management::CodeReference codeReference({line_index , begin_index} , {line_index , current_index - 1}) ;
                    string_view currentToken = currentLine.substr(begin_index , current_index - begin_index) ;
                    assert(!currentToken.empty());
                    // check for type of valid token
                    if(isKeyword(currentToken))
                        streamTokens.emplace_back(codeReference , TokenType::KEYWORD) ;
                    else if(isIdentifier(currentToken))
                        streamTokens.emplace_back(codeReference , TokenType::IDENTIFIER) ;
                    else if(isLiteral(currentToken))
                        streamTokens.emplace_back(codeReference, TokenType::LITERAL) ;
                    else if(isValidSpecialChar(currentToken[0])) {
                        if(currentToken == ":=")
                            streamTokens.emplace_back(codeReference, TokenType::VAR_ASSIGNMENT) ;
                        // tokens of size = 1
                        else {
                            switch (currentToken[0]) {
                                case ',': streamTokens.emplace_back(codeReference, TokenType::COMMA_SEPARATOR); break;
                                case '.': streamTokens.emplace_back(codeReference, TokenType::TERMINATOR); break;
                                case ';': streamTokens.emplace_back(codeReference, TokenType::SEMI_COLON_SEPARATOR); break;
                                case '=': streamTokens.emplace_back(codeReference, TokenType::CONST_ASSIGNMENT); break;
                                case '+': streamTokens.emplace_back(codeReference, TokenType::PLUS_OPERATOR); break;
                                case '-': streamTokens.emplace_back(codeReference, TokenType::MINUS_OPERATOR); break;
                                case '*': streamTokens.emplace_back(codeReference, TokenType::MULTIPLY_OPERATOR);break;
                                case '/': streamTokens.emplace_back(codeReference, TokenType::DIVIDE_OPERATOR); break;
                                case '(': streamTokens.emplace_back(codeReference, TokenType::OPEN_BRACKET); break;
                                case ')':  streamTokens.emplace_back(codeReference, TokenType::CLOSE_BRACKET); break;
                                default:
                                // if token is invalid
                                {
                                    manager->printTokenFailure(codeReference); // error type -> unexpected token
                                    return false;
                                }
                            }
                        }
                    }
                    else
                    /// if token is invalid
                    {
                        manager->printTokenFailure(codeReference); // error type -> unexpected token
                        return false;
                    }

                    begin_index = current_index;
                }
                else
                /// if token is invalid
                {
                    manager->printTokenFailure({{line_index , begin_index} , {line_index , begin_index}}) ;
                    return false;
                }
            }
        }
    }
    return true ;
}
//---------------------------------------------------------------------------
TokenStream::Token TokenStream::nextToken() {
    Token token = lookup()  ;
    iterator_token++ ;
    return token ;
}
//---------------------------------------------------------------------------
TokenStream::Token TokenStream::lookup() const {
    assert(!isEmpty()) ;
    return streamTokens[iterator_token] ;
}
//---------------------------------------------------------------------------
bool TokenStream::isEmpty() const {
    return iterator_token == streamTokens.size() ;
}
//---------------------------------------------------------------------------
TokenStream::Token::Token(management::CodeReference reference, TokenStream::TokenType tokenType)
    : codeReference(std::move(reference)) , type(tokenType){}
//---------------------------------------------------------------------------
management::CodeReference TokenStream::Token::getCodeReference() {
    return codeReference;
}
//---------------------------------------------------------------------------
TokenStream::TokenType TokenStream::Token::getTokenType() const {
    return type ;
}
//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------
