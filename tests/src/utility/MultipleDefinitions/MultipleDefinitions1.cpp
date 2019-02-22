
#include "MultipleDefinitions1.hpp"
#include "MultipleDefinitions2.hpp"

static int multipleDefinitionsSomeStaticVariable2 = 87;

int MulDefSomeClass::multipleDefinitionsGetValue2()
{
    return multipleDefinitionsAnotherGetValue();
}

int multipleDefinitionsCircularGetValue()
{
    return ++multipleDefinitionsSomeStaticVariable2;
}
