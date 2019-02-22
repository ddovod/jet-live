
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/UndefinedSymbolDeep.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Undefined symbols after reload, deep symbols dependency", "[common]")
{
    auto beforeReload = undefinedDeepGetValue();
    REQUIRE(beforeReload == 33);

    std::cout << "JET_TEST: disable(undefined_sym_deep:1); enable(undefined_sym_deep:2)" << std::endl;
    waitForReload();

    auto afterReload = undefinedDeepGetValue();
    REQUIRE(afterReload == 56);
}
