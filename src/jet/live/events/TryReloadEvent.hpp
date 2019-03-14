
#pragma once

#include "jet/live/IEvent.hpp"

namespace jet
{
    class TryReloadEvent : public IEvent
    {
    public:
        TryReloadEvent()
            : IEvent(EventType::kTryReload)
        {
        }
    };
}
