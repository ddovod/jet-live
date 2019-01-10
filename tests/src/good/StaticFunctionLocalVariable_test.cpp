
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticFunctionLocalVariable.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of function local static variable", "[variable]")
{
    auto beforeReload = getNext2();
    REQUIRE(beforeReload == 4);

    std::cout << "JET_TEST: disable(13:1)" << std::endl;
    waitForReload();

    auto afterReload = getNext2();
    REQUIRE(afterReload == 8);
}
