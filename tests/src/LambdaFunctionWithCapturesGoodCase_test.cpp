
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "LambdaFunctionWithCapturesGoodCase.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload of lambda function with captured data and another lamda in this file, good case", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;
    auto lambda = createLambdaFunctionWithCapturesGoodCase();

    REQUIRE(lambda(v1, v2) == sum);

    std::cout << "JET_TEST: disable(10:1); enable(10:2)" << std::endl;
    waitForReload();

    REQUIRE(lambda(v1, v2) == mul);
}
