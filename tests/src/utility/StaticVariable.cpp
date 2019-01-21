
#include "StaticVariable.hpp"

static int staticVariable = 10;

std::pair<int, int> getNext()
{
    staticVariable += 1;
    staticVariable -= 1;
    return {0, staticVariable++}; // <jet_tag: rel_stat_var:1>
//    return {1, staticVariable++}; // <jet_tag: rel_stat_var:2>
}
