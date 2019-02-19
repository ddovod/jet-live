
#include "UndefinedSymbolDeep.hpp"
#include "UndefinedSymbolDeep/UndefinedSymbolDeep1.hpp"

static int undefinedDeepSomeStaticVariable = 32;

int undefinedDeepGetValue()
{
    return ++undefinedDeepSomeStaticVariable; // <jet_tag: undefined_sym_deep:1>
//    return UndefinedDeepSomeClass{}.undefinedDeepGetValue2(); // <jet_tag: undefined_sym_deep:2>
}
