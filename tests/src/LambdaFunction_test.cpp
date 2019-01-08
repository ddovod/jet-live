
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "LambdaFunction.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload of lambda function with no captured data", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;
    auto lambda = createLambdaFunction();

    REQUIRE(lambda(v1, v2) == sum);

    std::cout << "JET_TEST: disable(6:1); enable(6:2)" << std::endl;
    waitForReload();

    REQUIRE(lambda(v1, v2) == mul);
}
