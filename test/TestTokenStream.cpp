#include <gtest/gtest.h>
#include "pljit/CodeManager.hpp"
#include "pljit/TokenStream.hpp"

using namespace std;
using namespace jitcompiler ;

TEST(TestTokenStream , singleToken) {
    constexpr array<string_view, 6> keywords = {"PARAM", "VAR", "CONST", "BEGIN", "END", "RETURN"};
    constexpr array<pair<string_view , TokenStream::TokenType>, 11> specialChars =
        {
            make_pair("." , TokenStream::TokenType::TERMINATOR) ,
            make_pair("," , TokenStream::TokenType::COMMA_SEPARATOR),
            make_pair(";" , TokenStream::TokenType::SEMI_COLON_SEPARATOR),
            make_pair(":=" , TokenStream::TokenType::VAR_ASSIGNMENT),
            make_pair("+" , TokenStream::TokenType::PLUS_OPERATOR),
            make_pair("-" , TokenStream::TokenType::MINUS_OPERATOR),
            make_pair("*" , TokenStream::TokenType::MULTIPLY_OPERATOR),
            make_pair("/" , TokenStream::TokenType::DIVIDE_OPERATOR),
            make_pair("=" , TokenStream::TokenType::CONST_ASSIGNMENT),
            make_pair("(" , TokenStream::TokenType::OPEN_BRACKET),
            make_pair(")" , TokenStream::TokenType::CLOSE_BRACKET)
        };
    constexpr array<string_view , 6> identifiers = {"cUrreNt" , "InDex" , "wIdth" , "Node" , "first" , "last"};
    constexpr array<string_view , 6> literals = {"10" , "00" , "2182198" , "540" , "210" , "01234567890"};
    // keywords
    for(const auto &cur : keywords) {
        CodeManager codeManager(cur) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(!codeManager.isCodeError()) ;
        ASSERT_TRUE(!lexicalAnalyzer.isEmpty()) ;
        TokenStream::Token token = lexicalAnalyzer.nextToken() ;
        ASSERT_TRUE(lexicalAnalyzer.isEmpty()) ;
        string_view line = codeManager.getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
        size_t start_index = token.getCodeReference().getStartLineRange().second ;
        size_t last_index = token.getCodeReference().getEndLineRange().second ;
        ASSERT_EQ(line.substr(start_index , last_index - start_index + 1) , cur) ;
        ASSERT_EQ(TokenStream::TokenType::KEYWORD , token.getTokenType()) ;
    }
    // special token
    for(const auto &[cur , type] : specialChars) {
        CodeManager codeManager(cur) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(!codeManager.isCodeError()) ;
        ASSERT_TRUE(!lexicalAnalyzer.isEmpty()) ;
        TokenStream::Token token = lexicalAnalyzer.nextToken() ;
        ASSERT_TRUE(lexicalAnalyzer.isEmpty()) ;
        string_view line = codeManager.getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
        size_t start_index = token.getCodeReference().getStartLineRange().second ;
        size_t last_index = token.getCodeReference().getEndLineRange().second ;
        ASSERT_EQ(line.substr(start_index , last_index - start_index + 1) , cur) ;
        ASSERT_EQ(type , token.getTokenType()) ;
    }
    // identifiers
    for(const auto &cur : identifiers) {
        CodeManager codeManager(cur) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(!codeManager.isCodeError()) ;
        ASSERT_TRUE(!lexicalAnalyzer.isEmpty()) ;
        TokenStream::Token token = lexicalAnalyzer.nextToken() ;
        ASSERT_TRUE(lexicalAnalyzer.isEmpty()) ;
        string_view line = codeManager.getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
        size_t start_index = token.getCodeReference().getStartLineRange().second ;
        size_t last_index = token.getCodeReference().getEndLineRange().second ;
        ASSERT_EQ(line.substr(start_index , last_index - start_index + 1) , cur) ;
        ASSERT_EQ(TokenStream::TokenType::IDENTIFIER , token.getTokenType()) ;
    }
    // literals
    for(const auto &cur : literals) {
        CodeManager codeManager(cur) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(!codeManager.isCodeError()) ;
        ASSERT_TRUE(!lexicalAnalyzer.isEmpty()) ;
        TokenStream::Token token = lexicalAnalyzer.nextToken() ;
        ASSERT_TRUE(lexicalAnalyzer.isEmpty()) ;
        string_view line = codeManager.getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
        size_t start_index = token.getCodeReference().getStartLineRange().second ;
        size_t last_index = token.getCodeReference().getEndLineRange().second ;
        ASSERT_EQ(line.substr(start_index , last_index - start_index + 1) , cur) ;
        ASSERT_EQ(TokenStream::TokenType::LITERAL , token.getTokenType()) ;
    }
}
TEST(TestTokenStream , errorMessages){
    // invalid character
    {
        string source_code1 = "1213133?-29212\n";
        string errorMessage = "1:8: error: invalid token \"?\"\n"
                              "1213133?-29212\n"
                              "       ^\n" ;
        CodeManager codeManager(source_code1) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(codeManager.isCodeError()) ;
        ASSERT_EQ(errorMessage , codeManager.error_message()) ;
    }
    // invalid character
    {
        string source_code2 = "PARAM width, height, depth;\n"
                              "VAR vol#me\n"
                              "CONST density = 2400;\n"
                              "BEGIN\n"
                              "volume := width * height * depth;\n"
                              "RETURN density * volume\n"
                              "END.";
        string errorMessage = "2:8: error: invalid token \"#\"\n"
                              "VAR vol#me\n"
                              "       ^\n" ;
        CodeManager codeManager(source_code2) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(codeManager.isCodeError()) ;
        ASSERT_EQ(errorMessage , codeManager.error_message()) ;
    }
    // invalid character
    {
        string source_code3 = "PARAM width, height, depth;\n"
                              "VAR volume\n"
                              "CONST density = 2400;\n"
                              "BEGIN\n"
                              "volume := width * height * depth;\n"
                              "RETURN density * volume\n"
                              "END@\n";
        string errorMessage = "7:4: error: invalid token \"@\"\n"
                              "END@\n"
                              "   ^\n" ;
        CodeManager codeManager(source_code3) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(codeManager.isCodeError()) ;
        ASSERT_EQ(errorMessage , codeManager.error_message()) ;
    }
    // concatenate literal with identifier
    {
        string source_code4 = ":=,.;123a" ;
        string errorMessage = "1:6: error: invalid token \"123a\"\n"
                              ":=,.;123a\n"
                              "     ^~~~\n" ;
        CodeManager codeManager(source_code4) ;
        TokenStream lexicalAnalyzer(&codeManager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(codeManager.isCodeError()) ;
        ASSERT_EQ(errorMessage , codeManager.error_message()) ;
    }

}
TEST(TestTokenStream , actualCode) {
    {
        // compile error however it will pass lexical analysis
        string source_code_1 = "PARAM length, width;\n"
                               "VAR areaRectangle ;\n"
                               "BEGIN\n"
                               "\tareaRectangle= length *width;\n"
                               "\tRETURN areaRectangle\n"
                               "END.\n"
                               "        \n"
                               " \n"
                               "                       \n" ;
        constexpr array<pair<string_view , TokenStream::TokenType> , 19> tokens = {
            make_pair("PARAM" , TokenStream::TokenType::KEYWORD),
            make_pair("length" , TokenStream::TokenType::IDENTIFIER),
            make_pair("," , TokenStream::TokenType::COMMA_SEPARATOR),
            make_pair("width" , TokenStream::TokenType::IDENTIFIER),
            make_pair(";" , TokenStream::TokenType::SEMI_COLON_SEPARATOR),
            make_pair("VAR" , TokenStream::TokenType::KEYWORD),
            make_pair("areaRectangle" , TokenStream::TokenType::IDENTIFIER),
            make_pair(";" , TokenStream::TokenType::SEMI_COLON_SEPARATOR),
            make_pair("BEGIN" , TokenStream::TokenType::KEYWORD),
            make_pair("areaRectangle" , TokenStream::TokenType::IDENTIFIER) ,
            make_pair("=" , TokenStream::TokenType::CONST_ASSIGNMENT) ,
            make_pair("length" , TokenStream::TokenType::IDENTIFIER) ,
            make_pair("*" , TokenStream::TokenType::MULTIPLY_OPERATOR) ,
            make_pair("width" , TokenStream::TokenType::IDENTIFIER) ,
            make_pair(";" , TokenStream::TokenType::SEMI_COLON_SEPARATOR) ,
            make_pair("RETURN" , TokenStream::TokenType::KEYWORD) ,
            make_pair("areaRectangle" , TokenStream::TokenType::IDENTIFIER) ,
            make_pair("END" , TokenStream::TokenType::KEYWORD) ,
            make_pair("." , TokenStream::TokenType::TERMINATOR) ,
        };
        CodeManager manager(source_code_1) ;
        TokenStream lexicalAnalyzer(&manager) ;
        lexicalAnalyzer.compileCode() ;
        ASSERT_TRUE(!manager.isCodeError()) ;
        ASSERT_TRUE(!lexicalAnalyzer.isEmpty()) ;
        size_t tokenIndex = 0 ;
        while(!lexicalAnalyzer.isEmpty()){
            ASSERT_LT(tokenIndex , tokens.size()) ;
            TokenStream::Token token = lexicalAnalyzer.lookup() ;
            string_view line = manager.getCurrentLine(token.getCodeReference().getStartLineRange().first) ;
            size_t start_index = token.getCodeReference().getStartLineRange().second ;
            size_t last_index = token.getCodeReference().getEndLineRange().second ;

            ASSERT_EQ(tokens[tokenIndex].first , line.substr(start_index , last_index - start_index + 1));
            ASSERT_EQ(tokens[tokenIndex].second , token.getTokenType());
            lexicalAnalyzer.nextToken() ;
            tokenIndex++ ;
        }
        ASSERT_EQ(tokenIndex , tokens.size()) ;
    }
}