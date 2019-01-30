
#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>
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
        /**
         * Initialization is performed in a background thread.
         *
         */
        Live(std::unique_ptr<ILiveListener>&& listener = {}, const LiveConfig& config = {});
        ~Live();

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
         * Checks if initialization is finished.
         */
        bool isInitialized() const;

    private:
        std::unique_ptr<LiveContext> m_context;
        std::unique_ptr<FileWatcher> m_fileWatcher;
        std::unique_ptr<Compiler> m_compiler;
        int m_recreateFileWatcherAfterTicks = 0;
        std::thread m_initThread;
        std::atomic_bool m_initialized{false};

        void loadCompilationUnits();
        void loadSymbols();
        void loadExportedSymbols();
        void loadDependencies();
        void setupFileWatcher();
        void updateDependencies(CompilationUnit& cu);
        std::vector<std::string> getDirectoriesToMonitor();
    };
}
