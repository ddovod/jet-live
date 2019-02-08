
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/NewHeaderDependency.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Adding new header dependency in runtime", "[common]")
{
    int v1 = 52;
    int v2 = 65;
    int sum = v1 + v2;
    int mul = v1 * v2;

    REQUIRE(newHeaderDependencyComputeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(new_header:1); enable(new_header:2)" << std::endl;
    waitForReload();
    REQUIRE(newHeaderDependencyComputeResult(v1, v2) == mul);

    std::cout << "JET_TEST: disable(new_header:3); enable(new_header:4)" << std::endl;
    waitForReload();
    REQUIRE(newHeaderDependencyComputeResult(v1, v2) == mul + sum);
}
