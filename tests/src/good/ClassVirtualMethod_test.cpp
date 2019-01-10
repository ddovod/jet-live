
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/ClassVirtualMethod.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"
#include <jet/live/Utility.hpp>

TEST_CASE("Reload of class instance virtual method", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;
    auto ins = jet::make_unique<ClassVirtualMethod>();

    REQUIRE(ins->computeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(4:1); enable(4:2)" << std::endl;
    waitForReload();

    REQUIRE(ins->computeResult(v1, v2) == mul);
}
