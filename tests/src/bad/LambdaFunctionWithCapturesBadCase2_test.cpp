
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/LambdaFunctionWithCapturesBadCase2.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

TEST_CASE("Reload of lambda function with captured data and another lambda in the file with different signature", "[function]")
{
    int v1 = 23;
    int v2 = 45;
    int sum = v1 + v2;
    int mul = v1 * v2;
    auto lambda = createLambdaFunctionWithCapturesBadCase2();

    REQUIRE(lambda(v1, v2) == sum);

    std::cout << "JET_TEST: disable(lamb_capt_diff_sign_bad:1); enable(lamb_capt_diff_sign_bad:2)" << std::endl;
    waitForReload();

    REQUIRE_FALSE(lambda(v1, v2) == mul); // note REQUIRE_FALSE
}
