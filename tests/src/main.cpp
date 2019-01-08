
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <iostream>
#include <memory>
#include <jet/live/Live.hpp>
#include <jet/live/Utility.hpp>
#include "TestDelegate.hpp"

TestDelegate* g_testDelegatePtr = nullptr;
jet::Live* g_live = nullptr;

int main(int argc, char* argv[])
{
    std::cout.setf(std::ios::unitbuf);

    auto testDelegate = jet::make_unique<TestDelegate>();
    g_testDelegatePtr = testDelegate.get();

    auto live = jet::make_unique<jet::Live>(std::move(testDelegate));
    g_live = live.get();

    return Catch::Session().run(argc, argv);
}
