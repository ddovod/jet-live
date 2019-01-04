
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

        void tryReload();
        void update();

    private:
        std::unique_ptr<LiveContext> m_context;
        std::unique_ptr<FileWatcher> m_fileWatcher;
        std::unique_ptr<Compiler> m_compiler;

        void setupFileWatcher();
        void updateDependencies(const CompilationUnit& cu);
    };
}
