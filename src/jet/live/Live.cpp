
#include "Live.hpp"
#include <dlfcn.h>
#include <teenypath.h>
#include "jet/live/CodeReloadPipeline.hpp"
#include "jet/live/CompileCommandsCompilationUnitsParser.hpp"
#include "jet/live/DefaultProgramInfoLoader.hpp"
#include "jet/live/DefaultSymbolsFilter.hpp"
#include "jet/live/DepfileDependenciesHandler.hpp"
#include "jet/live/FunctionsHookingStep.hpp"
#include "jet/live/LinkTimeRelocationsStep.hpp"
#include "jet/live/SignalReloader.hpp"
#include "jet/live/StaticsCopyStep.hpp"
#include "jet/live/Utility.hpp"
#include "jet/live/events/FileChangedEvent.hpp"
#include "jet/live/events/TryReloadEvent.hpp"

namespace jet
{
    Live::~Live()
    {
        if (m_initThread.joinable()) {
            m_earlyExit = true;
            m_initThread.join();
        }
        onLiveDestroyed();
    }

    Live::Live(std::unique_ptr<ILiveListener>&& listener, const LiveConfig& config)
        : m_context(jet::make_unique<LiveContext>())
    {
        onLiveCreated(this, config.reloadOnSignal);

        m_context->liveConfig = config;
        m_context->listener = listener ? std::move(listener) : jet::make_unique<ILiveListener>();
        m_context->thisExecutablePath = getExecutablePath();
        m_context->linkerType = getSystemLinkerType(m_context.get());
        m_context->compilationUnitsParser = jet::make_unique<CompileCommandsCompilationUnitsParser>();
        m_context->dependenciesHandler = jet::make_unique<DepfileDependenciesHandler>();
        m_context->programInfoLoader = jet::make_unique<DefaultProgramInfoLoader>();
        m_context->symbolsFilter = jet::make_unique<DefaultSymbolsFilter>();
        m_context->events = jet::make_unique<AsyncEventQueue>();
        m_context->codeReloadPipeline = jet::make_unique<CodeReloadPipeline>();

        m_context->codeReloadPipeline->addStep(jet::make_unique<LinkTimeRelocationsStep>());
        m_context->codeReloadPipeline->addStep(jet::make_unique<FunctionsHookingStep>());
        m_context->codeReloadPipeline->addStep(jet::make_unique<StaticsCopyStep>());

        m_compiler = jet::make_unique<Compiler>(m_context.get());

        m_context->events->addLog(LogSeverity::kInfo, "Initializing...");
        // The most dumb way to perform initialization in background thread.
        // TODO(ddovod): rework this pls
        m_initThread = std::thread([this] {
            std::vector<std::function<void()>> initializationFunctions = {
                [this] { loadCompilationUnits(); },
                [this] { m_context->dirFilters = getDirectoryFilters(); },
                [this] { loadDependencies(); },
                [this] { setupFileWatcher(); },
                [this] { loadSymbols(); },
                [this] { loadExportedSymbols(); },
                [this] { m_initialized.store(true); },
            };

            for (auto& f : initializationFunctions) {
                f();
                if (m_earlyExit) {
                    break;
                }
            }
        });
    }

    void Live::update()
    {
        while (auto logEvent = m_context->events->getLogEvent()) {
            m_context->listener->onLog(logEvent->getSeverity(), logEvent->getMessage());
            m_context->events->popLogEvent();
        }

        if (!isInitialized()) {
            return;
        }
        if (m_initThread.joinable()) {
            m_initThread.join();
            m_context->events->addLog(LogSeverity::kInfo, "Ready");
        }

        if (m_recreateFileWatcherAfterTicks == 10) {
            m_fileWatcher.reset();
        }
        if (m_recreateFileWatcherAfterTicks > 0) {
            m_recreateFileWatcherAfterTicks--;
        }
        if (m_recreateFileWatcherAfterTicks == 1) {
            setupFileWatcher();
        }

        if (m_fileWatcher) {
            m_fileWatcher->update();
        }

        m_compiler->update();

        if (m_compiler->isLinking()) {
            // We should not perform any new compilation or
            // link tasks if link task is already running
            return;
        }

        while (auto event = m_context->events->getEvent()) {
            switch (event->getType()) {
                case EventType::kFileChanged: {
                    onFileChanged(static_cast<FileChangedEvent*>(event)->getFilepath());
                    break;
                }
                case EventType::kLog: break;  // already handled
                case EventType::kTryReload: {
                    tryReloadInternal();
                    break;
                }
                default: assert(false); break;  // smth went wrong
            }
            m_context->events->popEvent();
        }
    }

    bool Live::isInitialized() const { return m_initialized; }

    void Live::tryReload()
    {
        m_context->events->addLog(LogSeverity::kInfo, "Trying to reload code...");
        m_context->events->addEvent(jet::make_unique<TryReloadEvent>());
    }

    void Live::tryReloadInternal()
    {
        assert(isInitialized());
        m_compiler->link([this](int status,
                             const std::string& libPath,
                             const std::vector<std::string>& sourceFilePaths,
                             const std::string&) {
            m_context->listener->onCodePreLoad();

            if (status != 0) {
                m_context->listener->onCodePostLoad();
                return;
            }

            m_context->events->addLog(LogSeverity::kDebug, "Opening " + libPath + "...");
            auto libHandle = dlopen(libPath.c_str(), RTLD_NOW | RTLD_GLOBAL);  // NOLINT
            if (!libHandle) {
                m_context->events->addLog(
                    LogSeverity::kError, "Cannot open library " + libPath + "\n" + std::string(dlerror()));
                m_context->listener->onCodePostLoad();
                return;
            }
            m_context->events->addLog(LogSeverity::kDebug, "Library opened successfully");

            m_context->events->addLog(LogSeverity::kDebug, "Loading symbols from " + libPath + "...");
            auto libSymbols = m_context->programInfoLoader->getProgramSymbols(m_context.get(), libPath);
            m_context->events->addLog(LogSeverity::kDebug, "Symbols loaded");

            m_context->events->addLog(LogSeverity::kDebug, "Loading exported symbols list...");
            size_t totalExportedSymbols = 0;
            int totalFiles = 0;
            std::vector<std::string> objFilePaths;
            for (const auto& sourceFilePath : sourceFilePaths) {
                auto foundCu = m_context->compilationUnits.find(sourceFilePath);
                assert(foundCu != m_context->compilationUnits.end());
                if (foundCu == m_context->compilationUnits.end()) {
                    continue;
                }
                objFilePaths.push_back(foundCu->second.objFilePath);
                const auto& symNames =
                    m_context->programInfoLoader->getExportedSymbolNames(m_context.get(), foundCu->second.objFilePath);
                totalExportedSymbols += symNames.size();
                totalFiles++;
                for (const auto& el : symNames) {
                    m_context->exportedSymbolNamesInSourceFiles[el] = sourceFilePath;
                }
            }
            m_context->events->addLog(LogSeverity::kDebug,
                "Done, total exported symbols: " + std::to_string(totalExportedSymbols) + " in "
                    + std::to_string(totalFiles) + " files");

            Program libProgram;
            libProgram.path = libPath;
            libProgram.symbols = libSymbols;
            libProgram.objFilePaths = objFilePaths;

            m_context->codeReloadPipeline->reload(m_context.get(), &libProgram);

            m_context->events->addLog(LogSeverity::kInfo, "Code reloaded");
            m_context->programs.emplace_back(std::move(libProgram));
            m_context->listener->onCodePostLoad();
        });
    }

    void Live::updateDependencies(CompilationUnit& cu)
    {
        // Deps
        for (const auto& oldDep : m_context->dependencies[cu.sourceFilePath]) {
            m_context->inverseDependencies[oldDep].erase(cu.sourceFilePath);
        }
        auto cuDeps = m_context->dependenciesHandler->getDependencies(m_context.get(), cu);
        for (const auto& el : cuDeps) {
            m_context->inverseDependencies[el].insert(cu.sourceFilePath);
        }

        // File watcher
        if (m_fileWatcher) {
            for (const auto& el : cuDeps) {
                const auto& depDir = TeenyPath::path{el}.parent_path().string();
                auto found = m_context->dirsToMonitor.find(depDir);
                if (found == m_context->dirsToMonitor.end()) {
                    m_fileWatcher->addWatch(depDir);
                    m_context->dirsToMonitor.insert(depDir);
                }
            }
        }

        m_context->dependencies[cu.sourceFilePath] = std::move(cuDeps);
    }

    std::unordered_set<std::string> Live::getDirectoryFilters()
    {
        std::unordered_set<std::string> dirs;
        std::string commonDir;
        for (const auto& cu : m_context->compilationUnits) {
            const auto& sourceFilePath = cu.second.sourceFilePath;
            if (commonDir.empty()) {
                commonDir = sourceFilePath;
            } else {
                size_t minLength = std::min(commonDir.size(), sourceFilePath.size());
                for (size_t i = 0; i < minLength; i++) {
                    if (commonDir[i] != sourceFilePath[i]) {
                        commonDir = commonDir.substr(0, i);
                        break;
                    }
                }
            }
            auto p = TeenyPath::path(commonDir);
            if (!p.is_directory()) {
                commonDir = p.parent_path().string();
            }
        }
        m_context->events->addLog(LogSeverity::kDebug, "Root directory: " + commonDir);
        dirs.insert(commonDir);
        return dirs;
    }

    std::unordered_set<std::string> Live::getDirectoriesToMonitor()
    {
        std::unordered_set<std::string> dirsToMonitorSet;

        // Dependencies
        for (const auto& el : m_context->inverseDependencies) {
            dirsToMonitorSet.insert(TeenyPath::path{el.first}.parent_path().string());
        }

        // Other files
        for (const auto& el : m_context->compilationUnitsParser->getFilesToMonitor()) {
            dirsToMonitorSet.insert(TeenyPath::path{el}.parent_path().string());
        }

        return dirsToMonitorSet;
    }

    void Live::loadCompilationUnits()
    {
        m_context->events->addLog(LogSeverity::kDebug, "Parsing compilation commands...");
        m_context->compilationUnits = m_context->compilationUnitsParser->parseCompilationUnits(m_context.get());
        if (m_context->compilationUnits.empty()) {
            m_context->events->addLog(LogSeverity::kError, "There're no compilation units");
            return;
        }
        m_context->events->addLog(LogSeverity::kDebug,
            "Success parsing compilation commands, total " + std::to_string(m_context->compilationUnits.size())
                + " compilation units");
        m_context->events->addLog(LogSeverity::kInfo, "Load CUs: done");
    }

    void Live::loadSymbols()
    {
        for (const auto& el : m_context->programInfoLoader->getAllLoadedProgramsPaths(m_context.get())) {
            m_context->events->addLog(LogSeverity::kDebug, "Loading symbols for " + el + " ...");
            Program program;
            program.path = el;
            program.symbols = m_context->programInfoLoader->getProgramSymbols(m_context.get(), program.path);
            if (program.symbols.functions.empty() && program.symbols.variables.empty()) {
                m_context->events->addLog(LogSeverity::kDebug, el + " has no symbols, skipping");
                continue;
            }
            m_context->events->addLog(LogSeverity::kDebug,
                "Symbols loaded: funcs " + std::to_string(program.symbols.functions.size()) + ", vars "
                    + std::to_string(program.symbols.variables.size()) + ", "
                    + (el.empty() ? std::string("Self") : el));
            m_context->programs.push_back(std::move(program));
        }
        m_context->events->addLog(LogSeverity::kInfo, "Load symbols: done");
    }

    void Live::loadExportedSymbols()
    {
        m_context->events->addLog(LogSeverity::kDebug, "Loading exported symbols list...");
        size_t totalExportedSymbols = 0;
        int totalFiles = 0;
        for (const auto& cu : m_context->compilationUnits) {
            const auto& symNames =
                m_context->programInfoLoader->getExportedSymbolNames(m_context.get(), cu.second.objFilePath);
            totalExportedSymbols += symNames.size();
            totalFiles++;
            for (const auto& el : symNames) {
                m_context->exportedSymbolNamesInSourceFiles[el] = cu.first;
            }
        }
        m_context->events->addLog(LogSeverity::kDebug,
            "Done, total exported symbols: " + std::to_string(totalExportedSymbols) + " in "
                + std::to_string(totalFiles) + " files");
        m_context->events->addLog(LogSeverity::kInfo, "Load exported symbols: done");
    }

    void Live::loadDependencies()
    {
        m_context->events->addLog(LogSeverity::kDebug, "Parsing dependencies...");
        for (auto& cu : m_context->compilationUnits) {
            updateDependencies(cu.second);
        }
        m_context->events->addLog(LogSeverity::kDebug, "Success parsing dependencies");
        m_context->events->addLog(LogSeverity::kInfo, "Load dependencies: done");
    }

    void Live::setupFileWatcher()
    {
        m_context->dirsToMonitor = getDirectoriesToMonitor();
        m_fileWatcher = jet::make_unique<FileWatcher>(m_context->dirsToMonitor,
            [this](const FileWatcher::Event& event) {
                m_context->events->addEvent(jet::make_unique<FileChangedEvent>(event.directory + event.filename));
            },
            [](const std::string&, const std::string& f) {
                const auto s = f.size();
                // clang-format off
                    return (
                        // No '.o' object files
                        !(s > 1 && f[s - 2] == '.' && f[s - 1] == 'o') &&
                        // No '.tmp' cmake temp files
                        !(s > 3 && f[s - 4] == '.' && f[s - 3] == 't' && f[s - 2] == 'm' && f[s - 1] == 'p') &&
                        // No '.d' depfiles
                        !(s > 1 && f[s - 2] == '.' && f[s - 1] == 'd')
                        );
                // clang-format on
            });
        m_context->events->addLog(LogSeverity::kInfo, "Setup file watcher: done");
    }

    void Live::onFileChanged(const std::string& filepath)
    {
        auto foundDeps = m_context->inverseDependencies.find(filepath);
        if (foundDeps != m_context->inverseDependencies.end()) {
            for (const auto& depPath : foundDeps->second) {
                auto foundCu = m_context->compilationUnits.find(depPath);
                if (foundCu != m_context->compilationUnits.end()) {
                    auto& cu = foundCu->second;
                    m_compiler->compile(
                        cu, [this, &cu](int, const std::string&, const std::string&) { updateDependencies(cu); });
                } else {
                    m_context->events->addLog(LogSeverity::kError, "Cannot find dependency cu for " + depPath);
                }
            }
        } else {
            std::vector<std::string> addedCompilationUnits;
            std::vector<std::string> modifiedCompilationUnits;
            std::vector<std::string> removedCompilationUnits;
            auto changed = m_context->compilationUnitsParser->updateCompilationUnits(
                m_context.get(), filepath, &addedCompilationUnits, &modifiedCompilationUnits, &removedCompilationUnits);

            if (changed) {
                // Compiling all new and modified CUs
                for (const auto& cuList : {addedCompilationUnits, modifiedCompilationUnits}) {
                    for (const auto& cuPath : cuList) {
                        auto foundCu = m_context->compilationUnits.find(cuPath);
                        if (foundCu != m_context->compilationUnits.end()) {
                            auto& cu = foundCu->second;
                            m_compiler->compile(cu,
                                [this, &cu](int, const std::string&, const std::string&) { updateDependencies(cu); });
                        } else {
                            m_context->events->addLog(LogSeverity::kError, "Cannot find cu for " + cuPath);
                        }
                    }
                }

                // Removing removed CUs from everywhere
                for (const auto& cuPath : removedCompilationUnits) {
                    m_compiler->remove(cuPath);

                    for (const auto& oldDep : m_context->dependencies[cuPath]) {
                        m_context->inverseDependencies[oldDep].erase(cuPath);
                    }
                    m_context->dependencies.erase(cuPath);
                }

                // Setting up file watcher from scratch
                // We should do it after some time to let current file watcher
                // to release its' stuff
                // TODO(ddovod): need better runloop
                m_recreateFileWatcherAfterTicks = 10;
            }
        }
    }
}
