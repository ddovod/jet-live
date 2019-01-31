
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/ModifyFileDuringReload.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Bug caused by modifying some source file during code reload process", "[common]")
{
    int v1 = 28;
    int v2 = 7;
    int sum = v1 + v2;
    int mul = v1 * v2;
    int diff = v1 - v2;
    int div = v1 / v2;

    REQUIRE(mfdrComputeResult(v1, v2) == sum);

    std::cout << "JET_TEST: disable(mfdr:1); enable(mfdr:2)" << std::endl;
    runAfterDelayAndWaitForReload([] {
        g_live->tryReload();
        g_live->update();
        g_live->update();
        std::cout << "JET_TEST: disable(mfdr:2); enable(mfdr:3)" << std::endl;
    });
    REQUIRE(mfdrComputeResult(v1, v2) == mul);

    runAfterDelayAndWaitForReload([] {
        g_live->tryReload();
    });
    REQUIRE(mfdrComputeResult(v1, v2) == diff);


    
    std::cout << "JET_TEST: disable(mfdr:3); enable(mfdr:4)" << std::endl;
    runAfterDelayAndWaitForReload([] {
        g_live->tryReload();
        g_live->update();
        g_live->update();
        std::cout << "JET_TEST: disable(mfdr:4); enable(mfdr:1)" << std::endl;
    });
    REQUIRE(mfdrComputeResult(v1, v2) == div);

    runAfterDelayAndWaitForReload([] {
        g_live->tryReload();
        g_live->update();
    });
    REQUIRE(mfdrComputeResult(v1, v2) == sum);
}
