
#pragma once

#include <mutex>
#include <process.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "jet/live/DataTypes.hpp"
#include "jet/live/LiveContext.hpp"

namespace jet
{
    class Compiler
    {
    public:
        explicit Compiler(const LiveContext* context);
        void update();
        void compile(const CompilationUnit& cu,
            std::function<void(int, const std::string&, const std::string&)>&& finishCallback);
        void link(std::function<void(int, const std::string&, const std::string&)>&& finishCallback);

    private:
        const LiveContext* m_context;
        struct Task
        {
            std::unique_ptr<TinyProcessLib::Process> process;
            int exitCode = -1;
            bool hasColorDiagnosticsFlag = false;
            std::string filename;
            std::string cuOrLibFilepath;
            std::string objFilepath;
            std::string errMessage;
            std::function<void(int, const std::string&, const std::string&)> finishCallback;
        };

        struct ShortCompilationUnit
        {
            std::string sourceFilepath;
            std::string objFilepath;
        };

        struct PendingCompilationTask
        {
            CompilationUnit cu;
            std::function<void(int, const std::string&, const std::string&)> finishCallback;
        };

        std::mutex m_mutex;
        std::unordered_map<std::string, Task> m_runningCompilationTasks;
        std::vector<PendingCompilationTask> m_pendingCompilationTasks;
        std::unique_ptr<Task> m_runningLinkTask;
        std::unordered_map<std::string, ShortCompilationUnit> m_readyCompilationUnits;
        int m_currentLibIndex = 1;
        std::string m_workingDirectory;
        std::string m_compilerPath;

        bool m_shouldLink = false;
        std::function<void(int, const std::string&, const std::string&)> m_pendingLinkingFinishCallback;

        void doCompile(const CompilationUnit& cu,
            std::function<void(int, const std::string&, const std::string&)>&& finishCallback);
        void doLink(std::function<void(int, const std::string&, const std::string&)>&& finishCallback);
    };
}
