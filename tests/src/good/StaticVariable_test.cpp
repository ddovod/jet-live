
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/StaticVariable.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of static variable", "[variable]")
{
    auto beforeReload = getNext();
    REQUIRE(beforeReload.first == 0);
    REQUIRE(beforeReload.second == 10);

    std::cout << "JET_TEST: disable(rel_stat_var:1); enable(rel_stat_var:2)" << std::endl;
    waitForReload();

    auto afterReload = getNext();
    REQUIRE(afterReload.first == 1);
    REQUIRE(afterReload.second == 11);
}
