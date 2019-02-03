
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

        /**
         * \param directoriesToWatch A list of directories to watch.
         * \param callback Function wich is called when file was modified.
         * \param filterFunc Function which is called from the background thread,
         *                   it shoudl check if given file should be processed further.
         */
        explicit FileWatcher(const std::vector<std::string>& directoriesToWatch,
            std::function<void(const Event&)>&& callback,
            std::function<bool(const std::string&, const std::string&)>&& filterFunc);
        ~FileWatcher();

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
        std::function<bool(const std::string&, const std::string&)> m_filterFunction;
        std::unordered_map<std::string, time_point_t> m_modificationTimePoints;
        std::unordered_map<std::string, uint64_t> m_fileHashes;
        std::unique_ptr<efsw::FileWatcher> m_fileWatcher;
        std::unique_ptr<EfswListener> m_efswListener;
        std::mutex m_fileEventsMutex;
        std::vector<FileWatcher::Event> m_fileEvents;
        std::vector<efsw::WatchID> m_watchIds;
    };
}
