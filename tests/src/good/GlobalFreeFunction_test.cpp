
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/GlobalFreeFunction.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload of global free function", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;

    REQUIRE(computeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(1:1); enable(1:2)" << std::endl;
    waitForReload();

    REQUIRE(computeResult(v1, v2) == mul);
}
