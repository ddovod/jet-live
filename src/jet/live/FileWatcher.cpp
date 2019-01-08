
#include "FileWatcher.hpp"
#include <cassert>
#include <map>
#include <memory>
#include "jet/live/Utility.hpp"

namespace jet
{
    FileWatcher::EfswListener::EfswListener(const FileWatcher::EfswListener::callback_t& callback)
        : m_callback(callback)
    {
    }

    void FileWatcher::EfswListener::handleFileAction(efsw::WatchID id,
        const std::string& dir,
        const std::string& filename,
        efsw::Action action,
        std::string oldFilename)
    {
        m_callback(id, dir, filename, action, oldFilename);
    }
}

namespace jet
{
    FileWatcher::FileWatcher(const std::vector<std::string>& directoriesToWatch,
        std::function<void(const Event&)>&& callback)
        : m_callback(std::move(callback))
    {
        m_fileWatcher = jet::make_unique<efsw::FileWatcher>();
        m_fileWatcher->followSymlinks(true);
        m_fileWatcher->allowOutOfScopeLinks(true);

        m_efswListener = jet::make_unique<EfswListener>([this](efsw::WatchID,
                                                            const std::string& dir,
                                                            const std::string& filename,
                                                            efsw::Action action,
                                                            std::string oldFilename) {
            using namespace std::chrono;

            static const std::map<efsw::Action, Action> enumMapping = {
                {efsw::Actions::Add, Action::kAdded},
                {efsw::Actions::Moved, Action::kMoved},
                {efsw::Actions::Modified, Action::kModified},
                {efsw::Actions::Delete, Action::kDeleted},
            };
            auto foundAction = enumMapping.find(action);
            if (foundAction == enumMapping.end()) {
                assert(false);
                return;
            }

            // Sometimes system fires several file events on single file operation with short time interval.
            // We're trying to collapse several events into 1.
            auto now = steady_clock::now();

            // Remove obsolete time points
            struct TimePointKey
            {
                Action action;
                std::string filepath;
            };
            std::vector<TimePointKey> timePointsToRemove;
            for (const auto& el : m_actionTimePoints) {
                for (const auto& tp : el.second) {
                    if (now - tp.second > milliseconds(100)) {
                        timePointsToRemove.push_back({static_cast<Action>(el.first), tp.first});
                    }
                }
            }
            for (const auto& el : timePointsToRemove) {
                auto intAction = static_cast<int>(el.action);
                m_actionTimePoints[intAction].erase(el.filepath);
                if (m_actionTimePoints[intAction].empty()) {
                    m_actionTimePoints.erase(intAction);
                }
            }

            // Checking if new action is "fake" action
            auto fullFilepath = dir + filename;
            auto found1 = m_actionTimePoints.find(static_cast<int>(foundAction->second));
            if (found1 != m_actionTimePoints.end()) {
                auto found2 = found1->second.find(fullFilepath);
                if (found2 != found1->second.end()) {
                    // This is a fake action, ignoring
                    return;
                }
            }

            m_actionTimePoints[static_cast<int>(foundAction->second)][fullFilepath] = now;

            std::lock_guard<std::mutex> lock(m_fileEventsMutex);
            m_fileEvents.push_back({foundAction->second, dir, filename, oldFilename});
        });

        for (const auto& el : directoriesToWatch) {
            m_watchIds.push_back(m_fileWatcher->addWatch(el, m_efswListener.get(), true));
        }

        m_fileWatcher->watch();
    }

    FileWatcher::~FileWatcher()
    {
        for (auto watchId : m_watchIds) {
            m_fileWatcher->removeWatch(watchId);
        }
    }

    void FileWatcher::update()
    {
        std::lock_guard<std::mutex> lock(m_fileEventsMutex);
        for (const auto& event : m_fileEvents) {
            m_callback(event);
        }
        m_fileEvents.clear();
    }
}
