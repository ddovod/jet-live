
#include <catch.hpp>
#include <iostream>
#include <thread>
extern "C" {
#include "utility/CommonSection.h"
}
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Relocation of global variable from *COM* (__DATA.__common) section", "[variable]")
{
    auto beforeReload = getCommonSectionVar();
    auto beforeReloadAddress = getCommonSectionVarAddress();
    REQUIRE(beforeReload == 10);

    std::cout << "JET_TEST: enable(common_sect:1)" << std::endl;
    waitForReload();

    auto afterReload = getCommonSectionVar();
    auto afterReloadAddress = getCommonSectionVarAddress();
    REQUIRE(afterReload == 11);
    REQUIRE(beforeReloadAddress == afterReloadAddress);

    *reinterpret_cast<int*>(afterReloadAddress) = 20;
    auto afterModify = getCommonSectionVar();
    REQUIRE(afterModify == 20);
}
