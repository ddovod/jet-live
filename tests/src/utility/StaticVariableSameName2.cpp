
#include "StaticVariableSameName2.hpp"

static int staticVariableWithTheSameName = 110;

std::pair<int, int> getNextSameName2()
{
    // Ensuring that this file was reloaded
    return {10, staticVariableWithTheSameName++}; // <jet_tag: 17:1>
//    return {11, staticVariableWithTheSameName++}; // <jet_tag: 17:2>
}
