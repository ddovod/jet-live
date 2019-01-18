
#include "StaticVariableSameName1.hpp"

static int staticVariableWithTheSameName = 10;

std::pair<int, int> getNextSameName1()
{
    // Ensuring that this file was reloaded
    return {0, staticVariableWithTheSameName++}; // <jet_tag: st_var_same_name:1>
//    return {1, staticVariableWithTheSameName++}; // <jet_tag: st_var_same_name:2>
}

