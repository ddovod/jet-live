
#include "LambdaFunction.hpp"

std::function<int(int, int)> createLambdaFunction()
{
    return [] (int v1, int v2) {
        return v1 + v2; // <jet_tag: lamb_nocapt:1>
//        return v1 * v2; // <jet_tag: lamb_nocapt:2>
    };
}
