
#include "GlobalVariable.hpp"

int someGlobalVariable = 10;

std::pair<int, int> getNextGlobal1()
{
    someGlobalVariable += 1;
    someGlobalVariable -= 1;
    return {0, someGlobalVariable++}; // <jet_tag: glob_var:1>
//    return {1, someGlobalVariable++}; // <jet_tag: glob_var:2>
}
