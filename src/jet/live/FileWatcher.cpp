
#include "FileWatcher.hpp"
#include <cassert>
#include <fcntl.h>
#include <map>
#include <memory>
#include <teenypath.h>
#include <unistd.h>
#include <xxhash.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    FileWatcher::FileWatcher(const std::unordered_set<std::string>& directoriesToWatch,
        std::function<void(const Event&)>&& callback,
        std::function<bool(const std::string&, const std::string&)>&& filterFunc)
        : m_callback(std::move(callback))
        , m_filterFunction(std::move(filterFunc))
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

            if (action == efsw::Action::Delete) {
                return;
            }

            if (m_filterFunction && !m_filterFunction(dir, filename)) {
                return;
            }

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
            std::vector<std::string> timePointsToRemove;
            for (const auto& tp : m_modificationTimePoints) {
                if (now - tp.second > milliseconds(100)) {
                    timePointsToRemove.push_back(tp.first);
                }
            }
            for (const auto& el : timePointsToRemove) {
                m_modificationTimePoints.erase(el);
            }

            // Checking if new action is "fake" action
            auto fullFilepath = dir + filename;
            auto found = m_modificationTimePoints.find(fullFilepath);
            if (found != m_modificationTimePoints.end()) {
                // This is a fake action, ignoring
                return;
            }

            // // Checking hash of the file
            // struct stat fdStat;
            // auto fd = open(fullFilepath.c_str(), O_RDONLY);
            // if (fd < 0) {
            //     // Looks like file doesn't exist right now,
            //     // there will be one more file event, let's wait for it
            //     return;
            // } else {
            //     fstat(fd, &fdStat);
            //     auto fileSize = static_cast<size_t>(fdStat.st_size);
            //     auto memBlock = mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, fd, 0l);
            //     if (memBlock != MAP_FAILED) {
            //         auto hash = XXH64(memBlock, fileSize, 0);
            //         munmap(memBlock, fileSize);
            //         close(fd);
            //         auto hashFound = m_fileHashes.find(fullFilepath);
            //         if (hashFound != m_fileHashes.end() && hashFound->second == hash) {
            //             // File content didn't change
            //             return;
            //         } else if (hashFound == m_fileHashes.end()) {
            //             m_fileHashes[fullFilepath] = hash;
            //         } else {
            //             hashFound->second = hash;
            //         }
            //     } else {
            //         // Same here, let's wait for the next event
            //         return;
            //     }
            // }

            m_modificationTimePoints[fullFilepath] = now;
            std::lock_guard<std::mutex> lock(m_fileEventsMutex);
            m_fileEvents.push_back({foundAction->second, dir, filename, oldFilename});
        });

        for (const auto& el : directoriesToWatch) {
            addWatch(el);
        }

        m_fileWatcher->watch();
    }

    FileWatcher::~FileWatcher()
    {
        std::lock_guard<std::mutex> lock(m_fileEventsMutex);
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

    void FileWatcher::addWatch(const std::string& dir)
    {
        m_watchIds.push_back(m_fileWatcher->addWatch(dir, m_efswListener.get(), false));
    }
}
