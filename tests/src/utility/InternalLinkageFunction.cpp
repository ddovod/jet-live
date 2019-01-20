
#include "InternalLinkageFunction.hpp"

namespace
{
    int internalComputeResult(int v1, int v2)
    {
        return v1 + v2; // <jet_tag: intern_func:1>
//        return v1 * v2; // <jet_tag: intern_func:2>
    }
}

void* getInternalFunctionAddress()
{
    return reinterpret_cast<void*>(::internalComputeResult);
}

int computeResult1(int v1, int v2)
{
    return ::internalComputeResult(v1, v2);
}
