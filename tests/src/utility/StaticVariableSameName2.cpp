
#include "StaticVariableSameName2.hpp"

static int staticVariableWithTheSameName = 110;

std::pair<int, int> getNextSameName2()
{
    staticVariableWithTheSameName += 1;
    staticVariableWithTheSameName -= 1;
    // Ensuring that this file was reloaded
    return {10, staticVariableWithTheSameName++}; // <jet_tag: st_var_same_name:1>
//    return {11, staticVariableWithTheSameName++}; // <jet_tag: st_var_same_name:2>
}
