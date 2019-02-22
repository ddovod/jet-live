
#include "FailedCompilationCrash.hpp"

int failedCompilationComputeResult(int v1, int v2)
{
    return v1 + v2; // <jet_tag: failed_comp:1>
//    return v1 + v2; // <jet_tag: failed_comp:2>
//    not compilable code // <jet_tag: failed_comp:3>
//    return v1 * v2; // <jet_tag: failed_comp:4>
}
