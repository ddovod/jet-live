
#include "LambdaFunctionWithCaptures.hpp"

std::function<int(int, int)> createLambdaFunctionWithCaptures()
{
    int dummyInt1 = 45;
    int dummyInt2 = -45;

    return [dummyInt1, dummyInt2] (int v1, int v2) {
        return v1 + v2 + dummyInt1 + dummyInt2; // <jet_tag: 7:1>
//        return v1 * v2 + dummyInt1 + dummyInt2; // <jet_tag: 7:2>
    };
}
