
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticVariableAddress.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of static variable, comparing address", "[variable]")
{
    auto oldVariableAddress = getStaticVariableAddress();

    std::cout << "JET_TEST: disable(16:1)" << std::endl;
    waitForReload();

    auto newVariableAddress = getStaticVariableAddress();
    REQUIRE_FALSE(oldVariableAddress == newVariableAddress); // note REQUIRE_FALSE
}
