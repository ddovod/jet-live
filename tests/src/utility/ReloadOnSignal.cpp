
#include "ReloadOnSignal.hpp"

namespace
{
    int get42Value()
    {
        static int res = 42;
        return res;
    }

    int get64Value()
    {
        static int res = 64;
        return res;
    }
}

int reloadOnSignalGetValue()
{
    return ::get42Value(); // <jet_tag: reload_signal:1>
//    return ::get64Value(); // <jet_tag: reload_signal:2>
}
