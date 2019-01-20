
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/ExternGlobalVariable.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

int someExternGlobalVariable = 10;

TEST_CASE("Relocation of extern global variable", "[variable]")
{
    auto beforeReload = getNextGlobal();
    REQUIRE(beforeReload.first == 0);
    REQUIRE(beforeReload.second == 10);

    std::cout << "JET_TEST: disable(extern_glob_var:1); enable(extern_glob_var:2)" << std::endl;
    waitForReload();

    auto afterReload = getNextGlobal();
    REQUIRE(afterReload.first == 1);
    REQUIRE(afterReload.second == 11);
}
