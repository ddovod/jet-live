
#include "StaticFunctionLocalVariable.hpp"

int getNext2()
{
    static int variable = 0;
    variable += 4;
    variable += 1;
    variable -= 1;
    // Just to touch the file
    (void)variable; // <jet_tag: st_func_loc_var:1>
    return variable;
}
