
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/MacosFunctionOutOfScope.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Function address is out of readable memory", "[function]")
{
    REQUIRE(getStdStringValue() == "some string");

    std::cout << "JET_TEST: disable(fun_out_of_scope:1); enable(fun_out_of_scope:2)" << std::endl;
    waitForReload();

    REQUIRE(getStdStringValue() == "some another string");
    
    std::cout << "JET_TEST: disable(fun_out_of_scope:2); enable(fun_out_of_scope:1)" << std::endl;
    waitForReload();

    REQUIRE(getStdStringValue() == "some string");
}
