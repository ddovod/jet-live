
#include "StaticFunctionLocalVariable.hpp"

int getNext2()
{
    static int variable = 0;
    variable += 4;
    // Just to touch the file
    (void)variable; // <jet_tag: 13:1>
    return variable;
}
