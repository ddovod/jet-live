
#include "StaticVariable.hpp"

static int staticVariable = 10;

void* getVariableAddress()
{
    return reinterpret_cast<void*>(&staticVariable);
}

std::pair<int, int> getNext()
{
    // Ensuring that this file was reloaded
    return {0, staticVariable++}; // <jet_tag: 11:1>
//    return {1, staticVariable++}; // <jet_tag: 11:2>
}
