#include "pljit/Pljit.hpp"

#include <iostream>
#include <thread>

//---------------------------------------------------------------------------
using namespace std;
using namespace jitcompiler ;
//---------------------------------------------------------------------------

//struct PairHash {
//    size_t operator()(const pair<std::optional<int64_t>, string>& pair) const {
//        return hash<std::optional<int64_t>>()(pair.first) ^ hash<string>()(pair.second);
//    }
//};

int main() {
    string identifier =
        "PARAM yb;\n"
        "VAR x , y , d;\n"
        "CONST xa = 1 , xb = 2 , ya = 3;\n"
        "BEGIN\n"
        "x := (xa - xb) * (xa - xb);\n"
        "y := (ya - yb) * (ya - yb);\n"
        "d := x / y;\n"
        "RETURN d;\n"
        "x:= 10 + (3 - 11) * 11 / 0\n"
        "END.\n";
    Pljit pljit ;
    auto func = pljit.registerFunction(identifier) ;
    auto res = func({3}) ;
    if(!res.second.empty())
        cout << res.second << '\n' ;
    return 0 ;
}
//---------------------------------------------------------------------------
