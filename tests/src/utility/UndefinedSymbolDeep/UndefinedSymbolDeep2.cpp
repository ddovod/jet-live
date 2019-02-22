
#include "UndefinedSymbolDeep2.hpp"

static int undefinedDeeptaticVarForSomeFunction = 55;

int undefinedDeepGetValueFromSomeFunction()
{
    return ++undefinedDeeptaticVarForSomeFunction;
}
