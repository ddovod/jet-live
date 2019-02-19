
#include "Compiler.hpp"
#include <algorithm>
#include <cassert>
#include <teenypath.h>
#include "jet/live/BuildConfig.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    Compiler::Compiler(const LiveContext* context)
        : m_context(context)
    {
    }

    Compiler::~Compiler()
    {
        for (auto& el : m_runningCompilationTasks) {
            el.second.process->kill();
        }

        if (m_runningLinkTask) {
            m_runningLinkTask->process->kill();
        }
    }

    void Compiler::update()
    {
        std::vector<std::string> tasksToRemove;
        for (auto& el : m_runningCompilationTasks) {
            int status = 0;
            if (el.second.process->try_get_exit_status(status)) {
                if (status == 0) {
                    m_readyCompilationUnits[el.second.cuOrLibFilepath] = {
                        el.second.cuOrLibFilepath, el.second.objFilepath};
                    m_context->events->addLog(LogSeverity::kInfo, "Success: " + el.second.filename);
                } else {
                    std::string message = "Failed: " + el.second.filename;
                    if (el.second.hasColorDiagnosticsFlag) {
                        // Resetting color since clang doesn't do it himself
                        message += "\n\033[m";
                    } else {
                        message += "\n";
                    }
                    message += el.second.errMessage;
                    m_context->events->addLog(LogSeverity::kWarning, std::move(message));
                }

                tasksToRemove.push_back(el.first);
                el.second.finishCallback(status, el.second.cuOrLibFilepath, el.second.errMessage);
            }
        }

        for (const auto& el : tasksToRemove) {
            m_runningCompilationTasks.erase(el);
        }

        while (m_runningCompilationTasks.size() < m_context->liveConfig.workerThreadsCount) {
            if (m_pendingCompilationTasks.empty()) {
                break;
            }
            auto& pendingTask = m_pendingCompilationTasks.front();
            doCompile(pendingTask.cu, std::move(pendingTask.finishCallback));
            m_pendingCompilationTasks.erase(m_pendingCompilationTasks.begin());
        }

        if (m_runningCompilationTasks.empty() && m_shouldLink) {
            m_shouldLink = false;
            doLink(std::move(m_pendingLinkingFinishCallback));
        }

        if (m_runningLinkTask) {
            int status = 0;
            if (m_runningLinkTask->process->try_get_exit_status(status)) {
                if (status != 0) {
                    m_context->events->addLog(LogSeverity::kWarning,
                        "Link failed: " + m_runningLinkTask->cuOrLibFilepath + "\n" + m_runningLinkTask->errMessage);
                } else {
                    m_context->events->addLog(LogSeverity::kInfo, "Linked successfully");
                }

                std::vector<std::string> objFilePaths;
                for (const auto& el : m_readyCompilationUnits) {
                    objFilePaths.push_back(el.second.objFilepath);
                }
                m_runningLinkTask->linkFinishCallback(
                    status, m_runningLinkTask->cuOrLibFilepath, objFilePaths, m_runningLinkTask->errMessage);
                m_runningLinkTask.reset();

                if (status == 0) {
                    m_readyCompilationUnits.clear();
                }
            }
        }
    }

    void Compiler::compile(const CompilationUnit& cu,
        std::function<void(int, const std::string&, const std::string&)>&& finishCallback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(!m_shouldLink && !m_runningLinkTask);

        if (m_compilerPath.empty()) {
            m_compilerPath = cu.compilerPath;
        }

        auto found = m_runningCompilationTasks.find(cu.sourceFilePath);
        if (found != m_runningCompilationTasks.end()) {
            found->second.process->kill();
            m_runningCompilationTasks.erase(found);
        }

        PendingCompilationTask pendingTask;
        pendingTask.cu = cu;
        pendingTask.finishCallback = std::move(finishCallback);
        m_pendingCompilationTasks.erase(std::find_if(m_pendingCompilationTasks.begin(),
                                            m_pendingCompilationTasks.end(),
                                            [&pendingTask](const PendingCompilationTask& task) {
                                                return task.cu.sourceFilePath == pendingTask.cu.sourceFilePath;
                                            }),
            m_pendingCompilationTasks.end());
        m_pendingCompilationTasks.push_back(std::move(pendingTask));
    }

    void Compiler::link(
        std::function<void(int, const std::string&, const std::vector<std::string>&, const std::string&)>&&
            finishCallback)
    {
        if (!m_runningCompilationTasks.empty() || m_runningLinkTask) {
            m_shouldLink = true;
            m_pendingLinkingFinishCallback = std::move(finishCallback);
        } else {
            doLink(std::move(finishCallback));
        }
    }

    void Compiler::remove(const std::string& compilationUnitPath)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto found = m_runningCompilationTasks.find(compilationUnitPath);
        if (found != m_runningCompilationTasks.end()) {
            found->second.process->kill();
            m_runningCompilationTasks.erase(found);
        }

        auto newEnd = std::remove_if(m_pendingCompilationTasks.begin(),
            m_pendingCompilationTasks.end(),
            [&compilationUnitPath](
                const PendingCompilationTask& t) { return t.cu.sourceFilePath == compilationUnitPath; });
        m_pendingCompilationTasks.erase(newEnd, m_pendingCompilationTasks.end());

        for (const auto& readyCu : m_readyCompilationUnits) {
            if (readyCu.second.sourceFilepath == compilationUnitPath) {
                m_readyCompilationUnits.erase(readyCu.first);
                break;
            }
        }
    }

    void Compiler::doCompile(const CompilationUnit& cu,
        std::function<void(int, const std::string&, const std::string&)>&& finishCallback)
    {
        auto filename = TeenyPath::path{cu.sourceFilePath}.filename();
        m_context->events->addLog(LogSeverity::kInfo, "Compiling: " + filename);

        Task task;
        task.filename = filename;
        task.hasColorDiagnosticsFlag = cu.hasColorDiagnosticsFlag;
        task.finishCallback = std::move(finishCallback);
        task.cuOrLibFilepath = cu.sourceFilePath;
        task.objFilepath = cu.objFilePath;
        auto compilationCommandStr = cu.compilationCommandStr;
        compilationCommandStr.append(" -fPIC ");
        if (!cu.depFilePath.empty()) {
            compilationCommandStr.append(" -MD -MF ").append(cu.depFilePath);
        }
        auto sourceFilePath = cu.sourceFilePath;
        task.process = jet::make_unique<TinyProcessLib::Process>(
            compilationCommandStr, cu.compilationDirStr, nullptr, [this, sourceFilePath](const char* bytes, size_t n) {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto found = m_runningCompilationTasks.find(sourceFilePath);
                if (found == m_runningCompilationTasks.end()) {
                    return;
                }
                found->second.errMessage += std::string(bytes, n);
            });
        m_runningCompilationTasks[cu.sourceFilePath] = std::move(task);
    }

    void Compiler::doLink(
        std::function<void(int, const std::string&, const std::vector<std::string>&, const std::string&)>&&
            finishCallback)
    {
        if (m_readyCompilationUnits.empty()) {
            m_context->events->addLog(LogSeverity::kInfo, "Nothing to reload.");
            return;
        }
        m_context->events->addLog(LogSeverity::kInfo, "Linking...");

        std::string libName = "lib_reload" + std::to_string(m_currentLibIndex++) + ".so";

        // First, we have to check if there will be unresolved symbols
        std::unordered_set<std::string> undefinedSymbolNames;
        std::unordered_set<std::string> uniqueObjectFilePaths;
        for (const auto& cu : m_readyCompilationUnits) {
            uniqueObjectFilePaths.insert(cu.second.objFilepath);
            auto symNames = m_context->programInfoLoader->getUndefinedSymbolNames(m_context, cu.second.objFilepath);
            undefinedSymbolNames.insert(symNames.begin(), symNames.end());
        }
        while (!undefinedSymbolNames.empty()) {
            auto undefSymName = *undefinedSymbolNames.begin();
            undefinedSymbolNames.erase(undefSymName);

            // Checking if this symbol can be resolved in load-time
            bool found = false;
            for (const auto& program : m_context->programs) {
                auto foundSym = program.symbols.exportedSymbolNames.find(undefSymName);
                if (foundSym != program.symbols.exportedSymbolNames.end()) {
                    found = true;
                    break;
                }
            }

            // If not, trying to find object file which defines this symbol
            if (!found) {
                auto foundObjectFilepath = m_context->exportedSymbolNamesInObjectFiles.find(undefSymName);
                if (foundObjectFilepath != m_context->exportedSymbolNamesInObjectFiles.end()) {
                    // If so, adding this object file in linkage list.
                    // Otherwise, lets hope for the best :/
                    if (uniqueObjectFilePaths.find(foundObjectFilepath->second) == uniqueObjectFilePaths.end()) {
                        uniqueObjectFilePaths.insert(foundObjectFilepath->second);
                        auto symNames = m_context->programInfoLoader->getUndefinedSymbolNames(
                            m_context, foundObjectFilepath->second);
                        undefinedSymbolNames.insert(symNames.begin(), symNames.end());
                    }
                }
            }
        }

        // Then linking all collected object files together
        std::vector<std::string> objectFilePaths;
        objectFilePaths.insert(objectFilePaths.end(), uniqueObjectFilePaths.begin(), uniqueObjectFilePaths.end());
        auto linkCommand = createLinkCommand(libName,
            m_compilerPath,
            findPrefferedBaseAddressForLibrary(objectFilePaths),
            m_context->linkerType,
            objectFilePaths);
        m_context->events->addLog(LogSeverity::kDebug, "Link command:\n" + linkCommand);

        const auto& buildDir = getCmakeBuildDirectory();
        Task task;
        task.linkFinishCallback = std::move(finishCallback);
        task.cuOrLibFilepath = buildDir + "/" + libName;
        task.process = jet::make_unique<TinyProcessLib::Process>(
            linkCommand, buildDir, nullptr, [this](const char* bytes, size_t n) {
                m_runningLinkTask->errMessage += std::string(bytes, n);
            });
        assert(!m_runningLinkTask);
        m_runningLinkTask = jet::make_unique<Task>(std::move(task));
    }

    bool Compiler::isLinking() const { return static_cast<bool>(m_runningLinkTask); }
}
