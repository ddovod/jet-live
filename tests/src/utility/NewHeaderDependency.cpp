
#include "NewHeaderDependency.hpp"
//#include "NewHeaderDependency/NewHeaderDependency2.hpp" // <jet_tag: new_header:2>

int newHeaderDependencyComputeResult(int v1, int v2)
{
    return v1 + v2; // <jet_tag: new_header:1>
//    return newHeaderDependency2ComputeResult(v1, v2); // <jet_tag: new_header:2>
}
