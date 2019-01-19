
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/GlobalVariable.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of global variable", "[variable]")
{
    auto beforeReload = getNextGlobal1();
    REQUIRE(beforeReload.first == 0);
    REQUIRE(beforeReload.second == 10);

    std::cout << "JET_TEST: disable(glob_var:1); enable(glob_var:2)" << std::endl;
    waitForReload();

    auto afterReload = getNextGlobal1();
    REQUIRE(afterReload.first == 1);
    REQUIRE(afterReload.second == 11);
}
