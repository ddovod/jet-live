
#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "jet/live/Compiler.hpp"
#include "jet/live/DataTypes.hpp"
#include "jet/live/FileWatcher.hpp"
#include "jet/live/LiveContext.hpp"

namespace jet
{
    class Live
    {
    public:
        explicit Live(std::unique_ptr<LiveDelegate>&& delegate = {});

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

    private:
        std::unique_ptr<LiveContext> m_context;
        std::unique_ptr<FileWatcher> m_fileWatcher;
        std::unique_ptr<Compiler> m_compiler;

        void setupFileWatcher();
        void updateDependencies(const CompilationUnit& cu);
    };
}
