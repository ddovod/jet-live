
#include "StaticVariableAddress.hpp"

static int staticVariableCheckingAddress = 10;

void* getStaticVariableAddress()
{
    (void)staticVariableCheckingAddress; // <jet_tag: st_var_addr:1>
    return reinterpret_cast<void*>(&staticVariableCheckingAddress);
}
