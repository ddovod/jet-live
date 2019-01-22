
#include "UndefinedSymbol2.hpp"

static int undefinedSomeStaticVariable2 = 42;

int UndefinedSomeClass::undefinedGetValue2()
{
    return undefinedSomeStaticVariable2++;
}
