
#pragma once

namespace jet
{
    class Live;

    void onLiveCreated(Live* live, bool reloadOnSignal);
    void onLiveDestroyed();
}
