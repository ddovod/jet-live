
#include "MultipleDefinitions.hpp"
#include "MultipleDefinitions/MultipleDefinitions1.hpp"

static int multipleDefinitionsSomeStaticVariable = 11;
static int multipleDefinitionsSomeStaticVariable2 = 87;

int multipleDefinitionsGetValue()
{
    return ++multipleDefinitionsSomeStaticVariable; // <jet_tag: mul_def:1>
//    return MulDefSomeClass{}.multipleDefinitionsGetValue2(); // <jet_tag: mul_def:2>
}

int multipleDefinitionsCircularGetValue()
{
    return ++multipleDefinitionsSomeStaticVariable2;
}
