
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/FailedCompilationCrash.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Crash when trying to reload code after success and then failed compilation", "[common]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;

    REQUIRE(failedCompilationComputeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(failed_comp:1); enable(failed_comp:2)" << std::endl;
    // waiting for compilation to finish
    int updatesCount = 0;
    while (true) {
        if (updatesCount == 30) {
            break;
        }
        g_live->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        updatesCount++;
    }

    std::cout << "JET_TEST: disable(failed_comp:2); enable(failed_comp:3)" << std::endl;
    waitForReload(); // fail, here should be a crash
    REQUIRE(failedCompilationComputeResult(v1, v2) == sum); // still old code
    
    std::cout << "JET_TEST: disable(failed_comp:3); enable(failed_comp:4)" << std::endl;
    waitForReload(); // success
    REQUIRE(failedCompilationComputeResult(v1, v2) == mul); // new code
}
