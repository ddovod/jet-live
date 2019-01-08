
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "InternalLinkageFunction.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload of function with internal linkage", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;
    using InternalFunctionPtrT = int (*)(int,int);
    auto internalFunctionPtr = reinterpret_cast<InternalFunctionPtrT>(getInternalFunctionAddress());

    REQUIRE(computeResult1(v1, v2) == sum);
    REQUIRE((*internalFunctionPtr)(v1, v2) == sum);

    std::cout << "JET_TEST: disable(5:1); enable(5:2)" << std::endl;
    waitForReload();

    REQUIRE(computeResult1(v1, v2) == mul);
    REQUIRE((*internalFunctionPtr)(v1, v2) == mul);
}
