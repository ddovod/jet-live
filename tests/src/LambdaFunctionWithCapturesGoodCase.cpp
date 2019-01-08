
#include "LambdaFunctionWithCapturesGoodCase.hpp"

std::function<int(int, int)> createLambdaFunctionWithCapturesGoodCase()
{
    int dummyInt1 = 45;
    int dummyInt2 = -45;
   auto dummyLambda = [dummyInt1, dummyInt2] (int v1, int v2) {
       return v1 + v2 - 123 + dummyInt1 - dummyInt2;
   };
   volatile int res = dummyLambda(55, 44);

    return [dummyInt1, dummyInt2] (int v1, int v2) {
        return v1 + v2 + dummyInt1 + dummyInt2; // <jet_tag: 10:1>
//        return v1 * v2 + dummyInt1 + dummyInt2; // <jet_tag: 10:2>
    };
}
