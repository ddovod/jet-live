
#include "UndefinedSymbol2.hpp"

static int undefinedSomeStaticVariable2 = 42;

int undefinedGetValue2()
{
    return undefinedSomeStaticVariable2++;
}
