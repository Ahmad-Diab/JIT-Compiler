#include "TokenStream.hpp"
#include <array>
#include <cassert>
//#include <iostream>
#include <optional>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace {

    bool isWhiteSpace(char c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ;}

    bool isDigit(char c) { return '0' <= c && c <= '9' ;}

    bool isLetter(char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'z') ;}

    bool isValidSpecialChar(char c) {
        return c == '.' || c == ',' || c == ';' || c == ':' || c == '+' || c == '-' || c == '/' || c == '*' ||
                c == '=' || c == '(' || c == ')' ;
    }

    bool isValidChar(char c) { return isDigit(c) || isLetter(c) || isValidSpecialChar(c) ; }

    bool isKeyword(string_view token) {
        constexpr array<string_view , 6> keywords = {"PARAM" , "VAR" , "CONST" , "BEGIN" , "END" , "RETURN"} ;
        for(const auto& cur : keywords)
            if(cur == token)
                return true ;
        return false ;
    }
    bool isIdentifer(string_view token) {
        for(char c : token)
            if(!isLetter(c))
                return false ;
        return true ;
    }

    bool isLiteral(string_view token) {
        for(char t : token)
            if(!isDigit(t))
                return false ;
        return true ;
    }
    optional<size_t> getNextIndex(string_view currentLine , size_t begin_index , size_t end_index) {
        size_t next_index = begin_index ;

        if(isLetter(currentLine[begin_index]) || isDigit(currentLine[begin_index])) {
            ++next_index ;
            while(next_index != end_index && (isLetter(currentLine[next_index]) || isDigit(currentLine[next_index])))
                ++next_index ;
            return next_index ;
        }
        else if(isValidChar(currentLine[begin_index])) {
            if(currentLine[begin_index] != ':')
                return next_index + 1 ;
            if(begin_index + 1 < end_index && currentLine[begin_index] == ':' && currentLine[begin_index + 1] == '=')
                return next_index + 2 ;
        }
        return std::nullopt ;
    }
} // namespace
namespace jitcompiler ::syntax{
//---------------------------------------------------------------------------
TokenStream::TokenStream(management::CodeManager* currentManager) : manager(currentManager)  {

}
void TokenStream::compileCode() {
    for(size_t line_index = 0 , num_lines = manager->countLines(); line_index < num_lines ; line_index++) {
        string_view currentLine = manager->getCurrentLine(line_index) ;
        for(size_t begin_index = 0 , line_size = currentLine.size() ; begin_index < line_size ; ++begin_index) {
            size_t end_index = begin_index ;
            while (end_index < line_size && !isWhiteSpace(currentLine[end_index]))
                ++end_index ;

            while (begin_index != end_index) {
                std::optional<size_t> next_index = getNextIndex(currentLine , begin_index , end_index) ;
                if(next_index) {
                    size_t current_index = next_index.value() ;
                    management::CodeReference codeReference({line_index , begin_index} , {line_index , current_index - 1}) ;
                    string_view currentToken = currentLine.substr(begin_index , current_index - begin_index) ;
                    assert(!currentToken.empty());
                    if(isKeyword(currentToken))
                        streamTokens.emplace_back(codeReference , TokenType::KEYWORD) ;
                    else if(isIdentifer(currentToken))
                        streamTokens.emplace_back(codeReference , TokenType::IDENTIFIER) ;
                    else if(isLiteral(currentToken))
                        streamTokens.emplace_back(codeReference, TokenType::LITERAL) ;
                    else if(isValidSpecialChar(currentToken[0])) {
                        if(currentToken == ":=")
                            streamTokens.emplace_back(codeReference, TokenType::VAR_ASSIGNMENT) ;
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
                                default: {
                                    manager->printTokenFailure(codeReference); // error type -> unexpected token
                                    return;
                                }
                            }
                        }
                    }
                    else
                    {
                        manager->printTokenFailure(codeReference); // error type -> unexpected token
                        return ;
                    }

                    begin_index = current_index;
                }
                else {
                    manager->printTokenFailure({{line_index , begin_index} , {line_index , begin_index}}) ;
                    return  ;
                }
            }
        }
    }

}
TokenStream::Token TokenStream::nextToken() {
    Token token = lookup()  ;
    iterator_token++ ;
    return token ;
}
TokenStream::Token TokenStream::lookup() const {
    return streamTokens[iterator_token] ;
}
bool TokenStream::isEmpty() const {
    return iterator_token == streamTokens.size() ;
}
management::CodeReference& TokenStream::Token::getCodeReference() {
    return codeReference;
}
TokenStream::TokenType TokenStream::Token::getTokenType() const {
    return type ;
}

//---------------------------------------------------------------------------
} // namespace jitcompiler::syntax
//---------------------------------------------------------------------------