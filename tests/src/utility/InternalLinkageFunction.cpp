
#include "InternalLinkageFunction.hpp"

namespace
{
    int internalComputeResult(int v1, int v2)
    {
        return v1 + v2; // <jet_tag: 5:1>
//        return v1 * v2; // <jet_tag: 5:2>
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
