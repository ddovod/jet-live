
#pragma once

#include <mutex>
#include <process.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "jet/live/DataTypes.hpp"
#include "jet/live/LiveContext.hpp"

namespace jet
{
    /**
     * Incapsulates compiler-related tasks like compilation and linkage.
     */
    class Compiler
    {
    public:
        explicit Compiler(const LiveContext* context);
        ~Compiler();

        /**
         * Runloop method, should be periodically called by the application.
         */
        void update();

        /**
         * Adds new compilation task into the queue.
         * If there's any running compilation process of this cu, or this cu is
         * already in the compilation queue, it will be stopped/removed and
         * new task will be created.
         * \param cu Compilation unit.
         * \param finishCallback Callback which will be called after compilation process finish.
         */
        void compile(const CompilationUnit& cu,
            std::function<void(int, const std::string&, const std::string&)>&& finishCallback);

        /**
         * Performs linkage of compiled compilation units.
         * It will wait for all pending and running compilation processes to finish
         * and then link newly compiled object files into shared library.
         * \param finishCallback Callback which will be called after linkage process finish.
         */
        void link(std::function<void(int, const std::string&, const std::vector<std::string>&, const std::string&)>&&
                finishCallback);

        /**
         * Removes given compilation unit from the compilation queue and ready obj files list.
         */
        void remove(const std::string& compilationUnitPath);

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
            std::function<void(int, const std::string&, const std::vector<std::string>&, const std::string&)>
                linkFinishCallback;
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
        std::function<void(int, const std::string&, const std::vector<std::string>&, const std::string&)>
            m_pendingLinkingFinishCallback;

        void doCompile(const CompilationUnit& cu,
            std::function<void(int, const std::string&, const std::string&)>&& finishCallback);
        void doLink(std::function<void(int, const std::string&, const std::vector<std::string>&, const std::string&)>&&
                finishCallback);
    };
}
