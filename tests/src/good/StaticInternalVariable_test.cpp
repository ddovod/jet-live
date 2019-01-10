
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "StaticInternalVariable.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of static internal variable", "[variable]")
{
    auto oldVariableAddress = getVariableAddress1();
    auto beforeReload = getNext1();
    REQUIRE(beforeReload.first == 0);
    REQUIRE(beforeReload.second == 0);

    std::cout << "JET_TEST: disable(12:1); enable(12:2)" << std::endl;
    waitForReload();

    auto newVariableAddress = getVariableAddress1();
    auto afterReload = getNext1();
    REQUIRE(afterReload.first == 1);
    REQUIRE(afterReload.second == 1);

    REQUIRE_FALSE(oldVariableAddress == newVariableAddress); // note REQUIRE_FALSE
}
