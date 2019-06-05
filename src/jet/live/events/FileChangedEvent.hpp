
#pragma once

#include <string>
#include "jet/live/IEvent.hpp"

namespace jet
{
    class FileChangedEvent : public IEvent
    {
    public:
        FileChangedEvent(const std::string& filepath)
            : IEvent(EventType::kFileChanged)
            , m_filepath(filepath)
        {
        }
        const std::string& getFilepath() const { return m_filepath; }
        int getPriority() const override { return 20; }

    private:
        std::string m_filepath;
    };
}
