
#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include "jet/live/IEvent.hpp"
#include "jet/live/ILiveListener.hpp"
#include "jet/live/events/LogEvent.hpp"

namespace jet
{
    class AsyncEventQueue
    {
    public:
        void addLog(LogSeverity severity, std::string&& message);
        LogEvent* getLogEvent();
        void popLogEvent();

        void addFileChanged(const std::string& filepath);
        IEvent* getEvent();
        void popEvent();

    private:
        std::mutex m_logQueueMutex;
        std::queue<std::unique_ptr<LogEvent>> m_logQueue;

        std::mutex m_queueMutex;
        std::queue<std::unique_ptr<IEvent>> m_queue;
    };
}
