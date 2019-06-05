
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

        void addEvent(std::unique_ptr<IEvent>&& event);
        IEvent* getEvent();
        void popEvent();

    private:
        struct QueueComparator
        {
            bool operator()(const std::unique_ptr<IEvent>& l, const std::unique_ptr<IEvent>& r)
            {
                return l->getPriority() < r->getPriority();
            }
        };
        template <typename T>
        using queue_t = std::priority_queue<T, std::vector<T>, QueueComparator>;

        std::mutex m_logQueueMutex;
        std::queue<std::unique_ptr<LogEvent>> m_logQueue;

        std::mutex m_queueMutex;
        queue_t<std::unique_ptr<IEvent>> m_queue;
    };
}
