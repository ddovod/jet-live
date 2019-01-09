
#include "StaticFunctionLocalVariable.hpp"

std::pair<void*, int> getNext2()
{
    static int variable = 0;
    variable += 4;
    // Just to touch the file
    (void)variable; // <jet_tag: 13:1>
    return {reinterpret_cast<void*>(&variable), variable};
}
