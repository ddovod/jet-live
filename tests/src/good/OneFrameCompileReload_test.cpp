
#include <catch.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include "utility/OneFrameCompileReload.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Checking crash if compilation and reload request appear in the same frame", "[common]")
{
    auto beforeReload = ofcrGetNext();
    REQUIRE(beforeReload == 4);

    std::cout << "JET_TEST: disable(ofcr_crash:1)" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    g_live->tryReload();
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    waitForReload();

    auto afterReload = ofcrGetNext();
    REQUIRE(afterReload == 8);
}
