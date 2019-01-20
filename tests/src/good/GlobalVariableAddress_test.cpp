
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/GlobalVariableAddress.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of global variable, comparing address", "[variable]")
{
    auto oldVariableAddress = getGlobalVariableAddress();

    std::cout << "JET_TEST: disable(glob_var_addr:1)" << std::endl;
    waitForReload();

    auto newVariableAddress = getGlobalVariableAddress();
    REQUIRE(oldVariableAddress == newVariableAddress);
}
