
#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <efsw/efsw.hpp>

namespace jet
{
    /**
     * Monitors directories and listens filesystem events.
     */
    class FileWatcher
    {
    public:
        enum class Action
        {
            kAdded,
            kMoved,
            kModified,
            kDeleted
        };

        struct Event
        {
            Action action;
            std::string directory;
            std::string filename;
            std::string oldFilename;
        };

        explicit FileWatcher(const std::vector<std::string>& directoriesToWatch,
            std::function<void(const Event&)>&& callback);

        void update();

    private:
        class EfswListener : public efsw::FileWatchListener
        {
        public:
            using callback_t =
                std::function<void(efsw::WatchID, const std::string&, const std::string&, efsw::Action, std::string)>;

            explicit EfswListener(const callback_t& callback);
            void handleFileAction(efsw::WatchID,
                const std::string&,
                const std::string&,
                efsw::Action,
                std::string) override;

        private:
            callback_t m_callback;
        };
        using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

        std::function<void(const Event&)> m_callback;
        std::unordered_map<int, std::unordered_map<std::string, time_point_t>> m_actionTimePoints;
        std::unique_ptr<efsw::FileWatcher> m_fileWatcher;
        std::unique_ptr<EfswListener> m_efswListener;
        std::mutex m_fileEventsMutex;
        std::vector<FileWatcher::Event> m_fileEvents;
    };
}
