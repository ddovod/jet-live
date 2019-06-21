
#include <catch.hpp>
#include "Globals.hpp"
#include "WaitForReload.hpp"
#include <iostream>
#include "utility/ArrayRelocation.hpp"

TEST_CASE("Relocation of array", "[variable]")
{
    arelTouchValues();
    
    auto beforeReload = arelGetValue();
    REQUIRE(beforeReload == 543);

    std::cout << "JET_TEST: disable(arel_var:1); enable(arel_var:2)" << std::endl;
    waitForReload();

    auto afterReload = arelGetValue();
    REQUIRE(afterReload == 878);
}
