
#include "StaticInternalVariableAddress.hpp"

namespace
{
    static int staticVariableCheckingAddress = 0;
}

void* getStaticInternalVariableAddress()
{
    (void)staticVariableCheckingAddress; // <jet_tag: 15:1>
    return reinterpret_cast<void*>(&staticVariableCheckingAddress);
}
