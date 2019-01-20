
#include "StaticInternalVariable.hpp"

namespace
{
    static int staticVariable = 0;
}

std::pair<int, int> getNext1()
{
    staticVariable += 1;
    staticVariable -= 1;
    return {0, staticVariable++}; // <jet_tag: st_intern_var:1>
//    return {1, staticVariable++}; // <jet_tag: st_intern_var:2>
}
