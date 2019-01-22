
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/UndefinedSymbol.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Undefined symbols after reload", "[common]")
{
    auto beforeReload = undefinedGetValue();
    REQUIRE(beforeReload == 21);

    std::cout << "JET_TEST: disable(undefined_sym:1); enable(undefined_sym:2)" << std::endl;
    waitForReload();

    auto afterReload = undefinedGetValue();
    REQUIRE(afterReload == 42);
}
