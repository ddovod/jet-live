
#pragma once

#include <map>

std::pair<int, int> severalGetGlobalVar();
std::pair<int, int> severalGetExternGlobalVar();
std::pair<int, int> severalGetStaticVar();
std::pair<int, int> severalGetInternalStaticVar();

void* severalGetGlobalVarAddress();
void* severalGetExternGlobalVarAddress();
void* severalGetStaticVarAddress();
void* severalGetInternalStaticAddress();
void* severalGetFunctionLocalStaticVarAddress();
