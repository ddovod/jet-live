
#include "utility/StaticFunctionLocalVariableAddress.hpp"

void* getStaticFunctionLocalVariableAddress()
{
    static int variable = 0;
    // Just to touch the file
    (void)variable; // <jet_tag: 14:1>
    return reinterpret_cast<void*>(&variable);
}
