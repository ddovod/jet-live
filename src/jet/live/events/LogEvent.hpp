
#pragma once

#include "jet/live/IEvent.hpp"
#include "jet/live/ILiveListener.hpp"

namespace jet
{
    class LogEvent : public IEvent
    {
    public:
        LogEvent(LogSeverity severity, std::string&& message)
            : IEvent(EventType::kLog)
            , m_severity(severity)
            , m_message(std::move(message))
        {
        }

        LogSeverity getSeverity() const { return m_severity; }
        const std::string& getMessage() const { return m_message; }
        int getPriority() const override { return 0; }

    private:
        LogSeverity m_severity;
        std::string m_message;
    };
}
