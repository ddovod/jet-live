
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <iostream>
#include <memory>
#include <jet/live/Live.hpp>
#include <jet/live/Utility.hpp>
#include "TestListener.hpp"

TestListener* g_testListenerPtr = nullptr;
jet::Live* g_live = nullptr;

int main(int argc, char* argv[])
{
    std::cout.setf(std::ios::unitbuf);

    auto testListener = jet::make_unique<TestListener>();
    g_testListenerPtr = testListener.get();

    auto live = jet::make_unique<jet::Live>(std::move(testListener));
    g_live = live.get();

    return Catch::Session().run(argc, argv);
}
