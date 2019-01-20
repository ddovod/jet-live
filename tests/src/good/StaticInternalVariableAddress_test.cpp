
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticInternalVariableAddress.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of static internal variable, comparing address", "[variable]")
{
    auto oldVariableAddress = getStaticInternalVariableAddress();

    std::cout << "JET_TEST: disable(st_intern_var_addr:1)" << std::endl;
    waitForReload();

    auto newVariableAddress = getStaticInternalVariableAddress();
    REQUIRE(oldVariableAddress == newVariableAddress);
}
