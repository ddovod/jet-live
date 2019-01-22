
#include "UndefinedSymbol.hpp"
#include "UndefinedSymbol2.hpp"

static int undefinedSomeStaticVariable = 21;

int undefinedGetValue()
{
    return undefinedSomeStaticVariable++; // <jet_tag: undefined_sym:1>
//    return UndefinedSomeClass{}.undefinedGetValue2(); // <jet_tag: undefined_sym:2>
}
