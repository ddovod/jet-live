
#include "SignalReloader.hpp"
#include <csignal>
#include <iostream>
#include "jet/live/Live.hpp"

namespace
{
    jet::Live* livePtr = nullptr;
}

void signalHandler(int)
{
    if (livePtr) {
        livePtr->tryReload();
    }
}

namespace jet
{
    void onLiveCreated(Live* live, bool reloadOnSignal)
    {
        ::livePtr = live;
        if (reloadOnSignal) {
            signal(SIGUSR1, signalHandler);
        }
    }

    void onLiveDestroyed() { ::livePtr = nullptr; }
}
