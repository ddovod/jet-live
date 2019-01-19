
#include "GlobalVariableAddress.hpp"

int someGlobalVariableCheckingAddress = 10;

void* getGlobalVariableAddress()
{
    someGlobalVariableCheckingAddress += 1;
    someGlobalVariableCheckingAddress -= 1;
    (void)someGlobalVariableCheckingAddress; // <jet_tag: glob_var_addr:1>
    return reinterpret_cast<void*>(&someGlobalVariableCheckingAddress);
}
