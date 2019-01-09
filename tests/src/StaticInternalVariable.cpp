
#include "StaticInternalVariable.hpp"

namespace
{
    static int staticVariable = 0;
}

void* getVariableAddress1()
{
    return reinterpret_cast<void*>(&staticVariable);
}

std::pair<int, int> getNext1()
{
    // Ensuring that this file was reloaded
    return {0, staticVariable++}; // <jet_tag: 12:1>
//    return {1, staticVariable++}; // <jet_tag: 12:2>
}
