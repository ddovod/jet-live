
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/ReloadOnSignal.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reloading code when SIGUSR1 is received", "[common]")
{
    auto beforeReload = reloadOnSignalGetValue();
    REQUIRE(beforeReload == 42);

    std::cout << "JET_TEST: disable(reload_signal:1); enable(reload_signal:2)" << std::endl;
    waitForReloadWithSignal();

    auto afterReload = reloadOnSignalGetValue();
    REQUIRE(afterReload == 64);
}
