
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/MultipleDefinitions.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Multiple definitions of symbols after reload, circular symbols dependency", "[common]")
{
    auto beforeReload = multipleDefinitionsGetValue();
    REQUIRE(beforeReload == 12);

    std::cout << "JET_TEST: disable(mul_def:1); enable(mul_def:2)" << std::endl;
    waitForReload();

    auto afterReload = multipleDefinitionsGetValue();
    REQUIRE(afterReload == 88);
}

