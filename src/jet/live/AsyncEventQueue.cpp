
#include "AsyncEventQueue.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    void AsyncEventQueue::addLog(LogSeverity severity, std::string&& message)
    {
        std::lock_guard<std::mutex> lock(m_logQueueMutex);
        m_logQueue.push(jet::make_unique<LogEvent>(severity, std::move(message)));
    }

    LogEvent* AsyncEventQueue::getLogEvent()
    {
        std::lock_guard<std::mutex> lock(m_logQueueMutex);
        return m_logQueue.empty() ? nullptr : m_logQueue.front().get();
    }

    void AsyncEventQueue::popLogEvent()
    {
        std::lock_guard<std::mutex> lock(m_logQueueMutex);
        if (!m_logQueue.empty()) {
            m_logQueue.pop();
        }
    }

    void AsyncEventQueue::addEvent(std::unique_ptr<IEvent>&& event)
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push(std::move(event));
    }

    IEvent* AsyncEventQueue::getEvent()
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        return m_queue.empty() ? nullptr : m_queue.top().get();
    }

    void AsyncEventQueue::popEvent()
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (!m_queue.empty()) {
            m_queue.pop();
        }
    }
}
