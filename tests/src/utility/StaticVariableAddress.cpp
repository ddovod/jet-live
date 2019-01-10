
#include "StaticVariableAddress.hpp"

static int staticVariableCheckingAddress = 10;

void* getStaticVariableAddress()
{
    (void)staticVariableCheckingAddress; // <jet_tag: 16:1>
    return reinterpret_cast<void*>(&staticVariableCheckingAddress);
}
