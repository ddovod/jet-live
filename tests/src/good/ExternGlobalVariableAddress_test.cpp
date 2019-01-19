
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/ExternGlobalVariableAddress.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

int externGlobalVariableCheckingAddress = 10;

TEST_CASE("Relocation of extern global variable, comparing address", "[variable]")
{
    auto oldVariableAddress = getExternGlobalVariableAddress();

    std::cout << "JET_TEST: disable(extern_glob_var_addr:1)" << std::endl;
    waitForReload();

    auto newVariableAddress = getExternGlobalVariableAddress();
    REQUIRE(oldVariableAddress == newVariableAddress);
}
