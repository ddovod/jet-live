
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/NewCompilationUnit.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Adding new compilation unit in runtime", "[common]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;

    REQUIRE(newCompilationUnitComputeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(new_cu:1); enable(new_cu:2)" << std::endl;
    waitForReload(5000);

    REQUIRE(newCompilationUnitComputeResult(v1, v2) == mul);
}
