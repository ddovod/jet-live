
#pragma once

namespace jet
{
    enum class EventType
    {
        kUnknown,
        kLog,
        kFileChanged,
        kTryReload,
    };

    class IEvent
    {
    public:
        explicit IEvent(EventType type)
            : m_type(type)
        {
        }
        virtual ~IEvent() {}
        EventType getType() const { return m_type; }

    private:
        EventType m_type = EventType::kUnknown;
    };
}
