#include "pljit/lib/TokenStream.h"
#include <array>
#include <cctype>
#include <optional>
//---------------------------------------------------------------------------
using namespace std ;
//---------------------------------------------------------------------------
namespace {
    constexpr array<string_view , 6> keywords = {"PARAM" , "VAR" , "CONST" , "BEGIN" , "END" , "RETURN"} ;

    bool isDigit(char c)
    {
        return '0' <= c && c <= '9' ;
    }
    bool isLetter(char c)
    {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'z') ;
    }
    bool isValidSpeicalChar(char c)
    {
        return c == '.' || c == ',' || c == ';' || c == ':' || c == '+' || c == '-' || c == '/' || c == '*' ||
                c == '=' || c == '(' || c == ')' ;
    }
    bool isValidChar(char c)
    {
        return isDigit(c) || isLetter(c) || isValidSpeicalChar(c) ;
    }
    bool isKeyword(string_view token)
    {
        for(const auto& cur : keywords)
            if(cur == token)
                return true ;
        return false ;
    }
    bool isIdentifer(string_view token)
    {
        for(char c : token)
            if(!isLetter(c))
                return false ;
        return true ;
    }
    bool isLiteral(string_view token)
    {
        size_t index = 0 ;
        while (index < token.size() && (token[index] == '+' || token[index] == '-'))
            index ++ ;
        if(index == token.size())
            return false ;
        for( ;index < token.size() ; index ++)
            if(!isDigit(token[index]))
                return false ;
        return true ;
    }

} // namespace
namespace jitcompiler {
//---------------------------------------------------------------------------

TokenStream::TokenStream(CodeManager* currentManager) : manager(currentManager) , isCompileError(false) {

    for(size_t line_index = 0 , num_lines = manager->countLines(); line_index < num_lines ; line_index++)
    {
        string_view currentLine = manager->getCurrentLine(line_index) ;

        for(size_t i = 0 , j = 0 , line_size = currentLine.size() ; i < line_size ; i++ , j++)
        {
            while (j < currentLine.size() && !std::isspace(static_cast<unsigned char>(currentLine[j]))) {
                if(!isValidChar(currentLine[j]))
                {
                    manager->printCompileError(line_index , j , j); // error type -> invalid character
                    isCompileError = true ;
                    return ;
                }
                j++;
            }
            if(j - i >= 1) {
                string_view currentToken = currentLine.substr(i, j - i);
                // line , start = i , end = j - 1 ,
                if(isKeyword(currentToken))
                    streamTokens.emplace_back(line_index , i , j - 1 , TokenType::KEYWORD) ;
                else if(isIdentifer(currentToken))
                    streamTokens.emplace_back(line_index , i , j - 1 , TokenType::IDENTIFIER) ;
                else if(isLiteral(currentToken))
                    streamTokens.emplace_back(line_index , i , j - 1 , TokenType::LITERAL) ;
                else if(j - i >= 2 && currentToken == ":=")
                    streamTokens.emplace_back(line_index , i , j - 1 , TokenType::VAR_ASSIGNMENT) ;
                else if(j - i >= 1)
                {
                    switch (currentToken[0]) {
                        case ',':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::COMMA_SEPARATOR) ;
                        }
                        break ;
                        case '.':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::TERMINATOR) ;
                        }
                        break ;
                        case ';':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::SEMI_COLON_SEPARATOR) ;
                        }
                        break ;
                        case '=':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::CONST_ASSIGNMENT) ;
                        }
                        break ;
                        case '+':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::PLUS_OPERATOR) ;
                        }
                        break ;
                        case '-':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::MINUS_OPERATOR) ;
                        }
                        break ;
                        case '*':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::MULTIPLY_OPERATOR) ;
                        }
                        break ;
                        case '/':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::DIVIDE_OPERATOR) ;
                        }
                        break ;
                        case '(':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::OPEN_BRACKET) ;
                        }
                        break ;
                        case ')':
                        {
                            streamTokens.emplace_back(line_index , i , j - 1 , TokenType::CLOSE_BRACKET) ;
                        }
                        break ;
                        default:
                        {
                            manager->printCompileError(line_index , i , j - 1); // error type -> unexpected token
                            isCompileError = true ;
                            return ;

                        }
                    }
                }
                else
                {
                    manager->printCompileError(line_index , i , j - 1); // error type -> unexpected token
                    isCompileError = true ;
                    return ;
                }
            }
            i = j ;
        }
    }
}
bool TokenStream::isInitialized() const {
    return !isCompileError;
}
void TokenStream::popNextToken() {
    iterator_token++ ;
}
TokenStream::Token TokenStream::getNextToken() const {
    return streamTokens[iterator_token] ;
}
bool TokenStream::isEmpty() const {
    return iterator_token == streamTokens.size() ;
}

//---------------------------------------------------------------------------
} // namespace jitcompiler
//---------------------------------------------------------------------------
