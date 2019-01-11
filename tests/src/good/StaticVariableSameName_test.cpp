
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticVariableSameName1.hpp"
#include "utility/StaticVariableSameName2.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of static variables with same names in different compilation units", "[variable]")
{
    auto beforeReload1 = getNextSameName1();
    auto beforeReload2 = getNextSameName2();
    REQUIRE(beforeReload1.first == 0);
    REQUIRE(beforeReload1.second == 10);
    REQUIRE(beforeReload2.first == 10);
    REQUIRE(beforeReload2.second == 110);

    std::cout << "JET_TEST: disable(17:1); enable(17:2)" << std::endl;
    waitForReload();

    auto afterReload1 = getNextSameName1();
    auto afterReload2 = getNextSameName2();
    REQUIRE(afterReload1.first == 1);
    REQUIRE(afterReload1.second == 11);
    REQUIRE(afterReload2.first == 11);
    REQUIRE(afterReload2.second == 111);
}
