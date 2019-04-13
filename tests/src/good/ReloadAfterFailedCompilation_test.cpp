#include <catch.hpp>
#include <iostream>
#include "utility/ReloadAfterFailedCompilation1.hpp"
#include "utility/ReloadAfterFailedCompilation2.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload after compilation error", "[common]")
{
    auto beforeReload1 = rafcGetValue1();
    auto beforeReload2 = rafcGetValue2();
    REQUIRE(beforeReload1 == 15);
    REQUIRE(beforeReload2 == 56);

    std::cout << "JET_TEST: disable(rafc:1); enable(rafc:2)" << std::endl;
    waitForReload(); // Getting compilation error, should not reload anything
    REQUIRE(beforeReload1 == rafcGetValue1());
    REQUIRE(beforeReload2 == rafcGetValue2());

    std::cout << "JET_TEST: disable(rafc:2); enable(rafc:3)" << std::endl;
    waitForReload(); // Now reload everything
    REQUIRE(rafcGetValue1() == 19);
    REQUIRE(rafcGetValue2() == 69);
}
