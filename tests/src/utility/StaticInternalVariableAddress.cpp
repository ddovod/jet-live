
#include "StaticInternalVariableAddress.hpp"

namespace
{
    static int staticVariableCheckingAddress = 0;
}

void* getStaticInternalVariableAddress()
{
    staticVariableCheckingAddress += 1;
    staticVariableCheckingAddress -= 1;
    (void)staticVariableCheckingAddress; // <jet_tag: st_intern_var_addr:1>
    return reinterpret_cast<void*>(&staticVariableCheckingAddress);
}
