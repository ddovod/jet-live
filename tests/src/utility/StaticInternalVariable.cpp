
#include "StaticInternalVariable.hpp"

namespace
{
    static int staticVariable = 0;
}

std::pair<int, int> getNext1()
{
    return {0, staticVariable++}; // <jet_tag: 12:1>
//    return {1, staticVariable++}; // <jet_tag: 12:2>
}
