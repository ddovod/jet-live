
#include <catch.hpp>
#include <iostream>
#include <thread>
#include "utility/SeveralReloads.hpp"
#include "Globals.hpp"
#include "WaitForReload.hpp"

int severalExternGlobalVariable = 100;
int severalExternGlobalVariableCheckingAddress = 0;

TEST_CASE("Several reloads", "[common]")
{
    struct Result
    {
        int globVarFirst;
        int globVarSecond;
        int externGlobalVarFirst;
        int externGlobalVarSecont;
        int staticVarFirst;
        int staticVarSecond;
        int internalStaticVarFirst;
        int internalStaticVarSecond;
    };

    std::vector<Result> expectedResults = {
        { 0, 10, 20, 100, 40, 25, 60, 56 },
        { 1, 11, 21, 101, 41, 26, 61, 57 },
        { 2, 12, 22, 102, 42, 27, 62, 58 },
        { 3, 13, 23, 103, 43, 28, 63, 59 },
        { 4, 14, 24, 104, 44, 29, 64, 60 },
        { 5, 15, 25, 105, 45, 30, 65, 61 },
        { 6, 16, 26, 106, 46, 31, 66, 62 },
        { 7, 17, 27, 107, 47, 32, 67, 63 },
        { 8, 18, 28, 108, 48, 33, 68, 64 },
    };
    void* expectedGlobVarAddress = severalGetGlobalVarAddress();
    void* expectedExternGlobVarAddress = severalGetExternGlobalVarAddress();
    void* expectedStaticVarAddress = severalGetStaticVarAddress();
    void* expectedInternalStaticVarAddress = severalGetStaticVarAddress();
    void* expectedFunctionLocalStaticVarAddress = severalGetFunctionLocalStaticVarAddress();

    for (int i = 0; i < 8; i++) {
        auto globVar = severalGetGlobalVar();
        auto externGlobVar = severalGetExternGlobalVar();
        auto staticVar = severalGetStaticVar();
        auto internalStaticVar = severalGetInternalStaticVar();
        auto globVarAddress = severalGetGlobalVarAddress();
        auto externGlobVarAddress = severalGetExternGlobalVarAddress();
        auto staticVarAddress = severalGetStaticVarAddress();
        auto internalStaticVarAddress = severalGetStaticVarAddress();
        auto functionLocalStaticVarAddress = severalGetFunctionLocalStaticVarAddress();
        
        const auto& res = expectedResults[i];
        REQUIRE(globVar.first == res.globVarFirst); REQUIRE(globVar.second == res.globVarSecond);
        REQUIRE(externGlobVar.first == res.externGlobalVarFirst); REQUIRE(externGlobVar.second == res.externGlobalVarSecont);
        REQUIRE(staticVar.first == res.staticVarFirst); REQUIRE(staticVar.second == res.staticVarSecond);
        REQUIRE(internalStaticVar.first == res.internalStaticVarFirst); REQUIRE(res.internalStaticVarSecond);
        REQUIRE(expectedGlobVarAddress == globVarAddress);
        REQUIRE(expectedExternGlobVarAddress == externGlobVarAddress);
        REQUIRE(expectedStaticVarAddress == staticVarAddress);
        REQUIRE(expectedInternalStaticVarAddress == internalStaticVarAddress);
        REQUIRE(expectedFunctionLocalStaticVarAddress == functionLocalStaticVarAddress);

        std::cout << "JET_TEST: disable(several:" << i + 1 << "); enable(several:" << i + 2 << ")" << std::endl;
        waitForReload();
    }
}
