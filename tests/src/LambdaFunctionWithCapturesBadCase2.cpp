
#include "LambdaFunctionWithCapturesBadCase2.hpp"

std::function<int(int, int)> createLambdaFunctionWithCapturesBadCase2()
{
   int dummyInt1 = 45;
   int dummyInt2 = -45;
//    auto dummyLambda = [dummyInt1, dummyInt2] (int v1, int v2, float v3) { // <jet_tag: 9:2>
//        return v1 + v2 - 123 + dummyInt1 - dummyInt2 + static_cast<int>(v3); // <jet_tag: 9:2>
//    }; // <jet_tag: 9:2>
//    volatile int res = dummyLambda(55, 44, 33.3f); // <jet_tag: 9:2>

    return [dummyInt1, dummyInt2] (int v1, int v2) {
        return v1 + v2 + dummyInt1 + dummyInt2; // <jet_tag: 9:1>
//        return v1 * v2 + dummyInt1 + dummyInt2; // <jet_tag: 9:2>
    };
}
