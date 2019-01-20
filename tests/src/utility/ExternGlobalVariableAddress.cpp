
#include "ExternGlobalVariableAddress.hpp"

extern int externGlobalVariableCheckingAddress;

void* getExternGlobalVariableAddress()
{
    externGlobalVariableCheckingAddress += 1;
    externGlobalVariableCheckingAddress -= 1;
    (void)externGlobalVariableCheckingAddress; // <jet_tag: extern_glob_var_addr:1>
    return reinterpret_cast<void*>(&externGlobalVariableCheckingAddress);
}
