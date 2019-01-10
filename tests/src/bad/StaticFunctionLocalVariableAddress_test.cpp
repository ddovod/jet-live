
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticFunctionLocalVariableAddress.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of function local static variable, comparing address", "[variable]")
{
    auto beforeReload = getStaticFunctionLocalVariableAddress();

    std::cout << "JET_TEST: disable(14:1)" << std::endl;
    waitForReload();

    auto afterReload = getStaticFunctionLocalVariableAddress();
    REQUIRE_FALSE(beforeReload == afterReload); // note REQUIRE_FALSE
}
