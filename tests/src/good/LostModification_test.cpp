
#include <catch.hpp>
#include <iostream>
#include "utility/LostModification.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Lost modifications when adding new file", "[common]")
{
    auto beforeReload = lostModificationGetValue();
    REQUIRE(beforeReload == 12);

    std::cout << "JET_TEST: disable(lost_mod:1); enable(lost_mod:2)" << std::endl;
    waitForReload(); // Load time error
    auto afterReload1 = lostModificationGetValue();
    REQUIRE(afterReload1 == 12);

    std::cout << "JET_TEST: enable(lost_mod:3)" << std::endl;
    waitForReload(); // 2 files should be reloaded
    auto afterReload2 = lostModificationGetValue();
    REQUIRE(afterReload2 == 34);
}
