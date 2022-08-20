#include <gtest/gtest.h>
#include <thread>
#include <mutex>
#include "pljit/Pljit.hpp"

using namespace std ;
using namespace jitcompiler ;
using namespace jitcompiler ::management;
using namespace jitcompiler ::syntax;
using namespace jitcompiler ::semantic;

TEST(TestPljit , TestLiteral) {
    Pljit pljit ;
    constexpr string_view code = "BEGIN\n"
                                 "RETURN 1\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    vector<int64_t > param ;
    auto result = func(param) ;
    ASSERT_TRUE(result.second.empty()) ;
    ASSERT_EQ(result.first.value() , 1) ;
}
TEST(TestPljit , TestIdentifier) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a;"
                                 "BEGIN\n"
                                 "RETURN a\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++) {
        vector<int64_t> param = {a};
        auto result = func(param);
        ASSERT_TRUE(result.second.empty());
        ASSERT_EQ(result.first.value(), a);
    }
}
TEST(TestPljit , TestUnaryPlus) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a;"
                                 "BEGIN\n"
                                 "RETURN +a\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++) {
        vector<int64_t> param = {a};
        auto result = func(param);
        ASSERT_TRUE(result.second.empty());
        ASSERT_EQ(result.first.value(), a);
    }
}
TEST(TestPljit , TestUnaryMinus) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a;"
                                 "BEGIN\n"
                                 "RETURN -a\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++) {
        vector<int64_t> param = {a};
        auto result = func(param);
        ASSERT_TRUE(result.second.empty());
        ASSERT_EQ(result.first.value(), -a);
    }
}
TEST(TestPljit , TestBinaryPlus) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a , b;"
                                 "BEGIN\n"
                                 "RETURN +a + -b\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++)
        for(int64_t b = -10 ; b <= 10  ; b++)
        {
            vector<int64_t> param = {a , b};
            auto result = func(param);
            ASSERT_TRUE(result.second.empty());
            ASSERT_EQ(result.first.value(), +a + -b);
        }
}

TEST(TestPljit , TestBinaryMinus) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a , b;"
                                 "BEGIN\n"
                                 "RETURN +a - -b\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++)
        for(int64_t b = -10 ; b <= 10  ; b++)
        {
            vector<int64_t> param = {a , b};
            auto result = func(param);
            ASSERT_TRUE(result.second.empty());
            ASSERT_EQ(result.first.value(), +a - -b);
        }
}
TEST(TestPljit , TestBinaryMult) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a , b;"
                                 "BEGIN\n"
                                 "RETURN +a * -b\n"
                                 "END.\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++)
        for(int64_t b = -10 ; b <= 10  ; b++)
        {
            vector<int64_t> param = {a , b};
            auto result = func(param);
            ASSERT_TRUE(result.second.empty());
            ASSERT_EQ(result.first.value(), +a * -b);
        }
}
TEST(TestPljit , TestBinaryDiv) {
    Pljit pljit ;
    constexpr string_view code = "PARAM a , b;"
                                 "BEGIN\n"
                                 "RETURN +a / -b\n"
                                 "END.\n" ;
    constexpr string_view expectedRuntimeError = "2:11: Runtime Error: Divide by Zero\n"
                                                 "RETURN +a / -b\n"
                                                 "          ^\n" ;
    auto func = pljit.registerFunction(code) ;
    for(int64_t a = -10 ; a <= 10 ; a++)
        for(int64_t b = -10 ; b <= 10  ; b++) {
            vector<int64_t> param = {a , b};
            auto result = func(param);
            if(b == 0) {
                ASSERT_TRUE(!result.second.empty());
                ASSERT_EQ(result.second,  expectedRuntimeError);
            }
            else {
                ASSERT_TRUE(result.second.empty());
                ASSERT_EQ(result.first.value(), +a / -b);
            }
        }
}

TEST(TestPljit , TestThreadSafe){
    constexpr string_view code =
        "PARAM a , b;"
        "BEGIN\n"
        "RETURN +a / -b\n"
        "END.\n" ;
    Pljit pljit ;
    auto func = pljit.registerFunction(code) ;
    std::unordered_map<int64_t , uint64_t> mpExpected ;
    for(int64_t a = 0 ; a <= 15 ; a ++)
        for(int64_t b = 1 ; b <= 10 ; b ++)
            mpExpected[a / -b]++;

    vector<thread> threads ;
    std::unordered_map<int64_t , uint64_t> mpThread ;
    std::mutex mapMutex ;

    for(int64_t a = 0 ; a <= 15 ; a++)
        for(int64_t b = 0 ; b <= 10 ; b++) {
            threads.emplace_back([&func , &mpThread , &mapMutex , a ,b] {
                vector<int64_t> param = {a, b};
                std::pair<std::optional<int64_t> , std::string> f = func(param);
                if(b > 0)
                {
                    ASSERT_TRUE(f.first.has_value()) ;
                    // since standard library is not thread safe
                    std::unique_lock lock(mapMutex) ;
                    mpThread[f.first.value()]++;
                }
                else {
                    ASSERT_TRUE(!f.first.has_value()) ;
                    ASSERT_EQ(f.second, "2:11: Runtime Error: Divide by Zero\n"
                                        "RETURN +a / -b\n"
                                        "          ^\n");
                }
            }) ;
        }
    for(auto &t : threads)
        t.join() ;

    ASSERT_EQ(mpThread.size() , mpExpected.size()) ;

    for(auto &[res , cnt] : mpExpected) {
        ASSERT_TRUE(mpThread.find(res) != mpThread.end()) ;
        ASSERT_EQ(mpThread[res] , cnt) ;
    }
}
TEST(TestPljit , TestHeavyMultipleCases) {
    constexpr string_view code = "PARAM yb;\n"
                                 "VAR x , y , d;\n"
                                 "CONST xa = 10 , xb = 40 , ya = 3;\n"
                                 "BEGIN\n"
                                 "x := (xa - xb) * (xa - xb);\n"
                                 "y := (ya - yb) * (ya - yb);\n"
                                 "d := x / y;\n"
                                 "RETURN d;\n"
                                 "x:= 10 + (3 - 11) * 11 / 0\n"
                                 "END.\n";

    Pljit pljit ;
    auto func = pljit.registerFunction(code) ;
    std::unordered_map<int64_t , uint64_t> mpExpected ;
    for(int64_t yb = 0 ; yb <= 15000 ; yb ++) {
        int64_t y = (3 - yb) * (3 - yb) ;
        if(y == 0) continue ;
        mpExpected[900 / y]++;
    }

    vector<thread> threads ;
    std::unordered_map<int64_t , uint64_t> mpThread ;
    std::mutex mapMutex ;

    for(int64_t yb = 0 ; yb <= 15000 ; yb++)
    {
        threads.emplace_back([&func , &mpThread , &mapMutex , yb] {
            std::pair<std::optional<int64_t> , std::string> f = func({yb});
            if(yb != 3)
            {
                ASSERT_TRUE(f.first.has_value()) ;
                // since standard library is not thread safe
                std::unique_lock lock(mapMutex) ;
                mpThread[f.first.value()]++;
            }
            else {
                ASSERT_TRUE(!f.first.has_value()) ;
                ASSERT_EQ(f.second, "7:8: Runtime Error: Divide by Zero\n"
                                    "d := x / y;\n"
                                    "       ^\n"
                );
            }
        }) ;
    }
    for(auto &t : threads)
        t.join() ;

    ASSERT_EQ(mpThread.size() , mpExpected.size()) ;

    for(auto &[res , cnt] : mpExpected) {
        ASSERT_TRUE(mpThread.find(res) != mpThread.end()) ;
        ASSERT_EQ(mpThread[res] , cnt) ;
    }
}