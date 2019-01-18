
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/ClassStaticMethod.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload of class static method", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;

    REQUIRE(ClassStaticMethod::computeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(cls_st_meth:1); enable(cls_st_meth:2)" << std::endl;
    waitForReload();

    REQUIRE(ClassStaticMethod::computeResult(v1, v2) == mul);
}
