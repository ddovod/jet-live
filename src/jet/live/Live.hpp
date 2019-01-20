
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "jet/live/Compiler.hpp"
#include "jet/live/FileWatcher.hpp"
#include "jet/live/ILiveListener.hpp"
#include "jet/live/LiveConfig.hpp"
#include "jet/live/LiveContext.hpp"

namespace jet
{
    struct CompilationUnit;

    /**
     * Main entry point of the library.
     * Ties together all the parts of functionality.
     */
    class Live
    {
    public:
        Live(std::unique_ptr<ILiveListener>&& listener = {}, const LiveConfig& config = {});

        /**
         * Tries to reload changed code.
         * It will wait for all pending compilation processes to finish.
         * Does nothing if there's no changes.
         * Call it only when you're done editing your code.
         */
        void tryReload();

        /**
         * Runloop method, should be periodically called by the application.
         */
        void update();

        /**
         * Prints some info, used mostly for debugging.
         */
        void printInfo();

    private:
        std::unique_ptr<LiveContext> m_context;
        std::unique_ptr<FileWatcher> m_fileWatcher;
        std::unique_ptr<Compiler> m_compiler;

        void setupFileWatcher();
        void updateDependencies(const CompilationUnit& cu);
        std::vector<std::string> getDirectoriesToMonitor();
    };
}
