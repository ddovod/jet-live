
#include "SeveralReloads.hpp"

static int severalStaticVariableCheckingAddress = 0;
static int severalStaticVariable = 25;
int severalGlobalVariable = 10;
int severalGlobalVariableCheckingAddress = 0;
extern int severalExternGlobalVariable;
extern int severalExternGlobalVariableCheckingAddress;

namespace
{
    static int severalInternalStaticVariableCheckingAddress = 0;
    static int severalInternalStaticVariable = 56;
}


std::pair<int, int> severalGetGlobalVar()
{
    severalGlobalVariable += 1;
    severalGlobalVariable -= 1;
    return {0, severalGlobalVariable++}; // <jet_tag: several:1>
//    return {1, severalGlobalVariable++}; // <jet_tag: several:2>
//    return {2, severalGlobalVariable++}; // <jet_tag: several:3>
//    return {3, severalGlobalVariable++}; // <jet_tag: several:4>
//    return {4, severalGlobalVariable++}; // <jet_tag: several:5>
//    return {5, severalGlobalVariable++}; // <jet_tag: several:6>
//    return {6, severalGlobalVariable++}; // <jet_tag: several:7>
//    return {7, severalGlobalVariable++}; // <jet_tag: several:8>
//    return {8, severalGlobalVariable++}; // <jet_tag: several:9>
}

std::pair<int, int> severalGetExternGlobalVar()
{
    severalExternGlobalVariable += 1;
    severalExternGlobalVariable -= 1;
    return {20, severalExternGlobalVariable++}; // <jet_tag: several:1>
//    return {21, severalExternGlobalVariable++}; // <jet_tag: several:2>
//    return {22, severalExternGlobalVariable++}; // <jet_tag: several:3>
//    return {23, severalExternGlobalVariable++}; // <jet_tag: several:4>
//    return {24, severalExternGlobalVariable++}; // <jet_tag: several:5>
//    return {25, severalExternGlobalVariable++}; // <jet_tag: several:6>
//    return {26, severalExternGlobalVariable++}; // <jet_tag: several:7>
//    return {27, severalExternGlobalVariable++}; // <jet_tag: several:8>
//    return {28, severalExternGlobalVariable++}; // <jet_tag: several:9>
}

std::pair<int, int> severalGetStaticVar()
{
    severalStaticVariable += 1;
    severalStaticVariable -= 1;
    return {40, severalStaticVariable++}; // <jet_tag: several:1>
//    return {41, severalStaticVariable++}; // <jet_tag: several:2>
//    return {42, severalStaticVariable++}; // <jet_tag: several:3>
//    return {43, severalStaticVariable++}; // <jet_tag: several:4>
//    return {44, severalStaticVariable++}; // <jet_tag: several:5>
//    return {45, severalStaticVariable++}; // <jet_tag: several:6>
//    return {46, severalStaticVariable++}; // <jet_tag: several:7>
//    return {47, severalStaticVariable++}; // <jet_tag: several:8>
//    return {48, severalStaticVariable++}; // <jet_tag: several:9>
}

std::pair<int, int> severalGetInternalStaticVar()
{
    severalInternalStaticVariable += 1;
    severalInternalStaticVariable -= 1;
    return {60, severalInternalStaticVariable++}; // <jet_tag: several:1>
//    return {61, severalInternalStaticVariable++}; // <jet_tag: several:2>
//    return {62, severalInternalStaticVariable++}; // <jet_tag: several:3>
//    return {63, severalInternalStaticVariable++}; // <jet_tag: several:4>
//    return {64, severalInternalStaticVariable++}; // <jet_tag: several:5>
//    return {65, severalInternalStaticVariable++}; // <jet_tag: several:6>
//    return {66, severalInternalStaticVariable++}; // <jet_tag: several:7>
//    return {67, severalInternalStaticVariable++}; // <jet_tag: several:8>
//    return {68, severalInternalStaticVariable++}; // <jet_tag: several:9>
}

void* severalGetGlobalVarAddress()
{
    severalGlobalVariableCheckingAddress += 1;
    severalGlobalVariableCheckingAddress -= 1;
    return reinterpret_cast<void*>(&severalGlobalVariableCheckingAddress);
}

void* severalGetExternGlobalVarAddress()
{
    severalExternGlobalVariableCheckingAddress += 1;
    severalExternGlobalVariableCheckingAddress -= 1;
    return reinterpret_cast<void*>(&severalExternGlobalVariableCheckingAddress);
}

void* severalGetStaticVarAddress()
{
    severalStaticVariableCheckingAddress += 1;
    severalStaticVariableCheckingAddress -= 1;
    return reinterpret_cast<void*>(&severalStaticVariableCheckingAddress);
}

void* severalGetInternalStaticAddress()
{
    severalInternalStaticVariableCheckingAddress += 1;
    severalInternalStaticVariableCheckingAddress -= 1;
    return reinterpret_cast<void*>(&severalInternalStaticVariableCheckingAddress);
}

void* severalGetFunctionLocalStaticVarAddress()
{
    static int severalFunctionLocalStaticVariable = 12;
    severalFunctionLocalStaticVariable += 1;
    severalFunctionLocalStaticVariable -= 1;
    return reinterpret_cast<void*>(&severalFunctionLocalStaticVariable);
}
