
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticInternalVariableAddress.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of static internal variable, comparing address", "[variable]")
{
    auto oldVariableAddress = getStaticInternalVariableAddress();

    std::cout << "JET_TEST: disable(15:1)" << std::endl;
    waitForReload();

    auto newVariableAddress = getStaticInternalVariableAddress();
    REQUIRE_FALSE(oldVariableAddress == newVariableAddress); // note REQUIRE_FALSE
}
