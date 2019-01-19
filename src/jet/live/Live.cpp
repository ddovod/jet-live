
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
#include "jet/live/StaticsCopyStep.hpp"
#include "jet/live/Utility.hpp"

bool shouldLogDebug = false;

namespace jet
{
    Live::Live(std::unique_ptr<ILiveListener>&& listener, const LiveConfig& config)
        : m_context(jet::make_unique<LiveContext>())
    {
        m_context->liveConfig = config;
        m_context->listener = listener ? std::move(listener) : jet::make_unique<ILiveListener>();
        m_context->thisExecutablePath = getExecutablePath();
        m_context->linkerType = getSystemLinkerType(m_context.get());
        m_context->compilationUnitsParser = jet::make_unique<CompileCommandsCompilationUnitsParser>();
        m_context->dependenciesHandler = jet::make_unique<DepfileDependenciesHandler>();
        m_context->programInfoLoader = jet::make_unique<DefaultProgramInfoLoader>();
        m_context->symbolsFilter = jet::make_unique<DefaultSymbolsFilter>();
        m_context->codeReloadPipeline = jet::make_unique<CodeReloadPipeline>();

        m_context->codeReloadPipeline->addStep(jet::make_unique<LinkTimeRelocationsStep>());
        m_context->codeReloadPipeline->addStep(jet::make_unique<FunctionsHookingStep>());
        m_context->codeReloadPipeline->addStep(jet::make_unique<StaticsCopyStep>());

        for (const auto& el : m_context->programInfoLoader->getAllLoadedProgramsPaths(m_context.get())) {
            Program program;
            program.path = el;
            program.symbols = m_context->programInfoLoader->getProgramSymbols(m_context.get(), program.path);
            if (program.symbols.functions.empty() && program.symbols.variables.empty()) {
                // Program has no symbols, skipping
                continue;
            }
            m_context->listener->onLog(LogSeverity::kInfo,
                "Symbols loaded: funcs " + std::to_string(program.symbols.functions.size()) + ", vars "
                    + std::to_string(program.symbols.variables.size()) + ", "
                    + (el.empty() ? std::string("Self") : el));
            m_context->programs.push_back(std::move(program));
        }

        shouldLogDebug = true;

        m_context->listener->onLog(LogSeverity::kInfo, "Parsing compilation commands...");
        m_context->compilationUnits = m_context->compilationUnitsParser->parseCompilationUnits(m_context.get());
        if (m_context->compilationUnits.empty()) {
            m_context->listener->onLog(LogSeverity::kError, "There're no compilation units");
            return;
        }

        m_context->listener->onLog(LogSeverity::kInfo,
            "Success parsing compilation commands, total " + std::to_string(m_context->compilationUnits.size())
                + " compilation units");

        setupFileWatcher();

        m_compiler = jet::make_unique<Compiler>(m_context.get());

        m_context->listener->onLog(LogSeverity::kInfo, "Parsing dependencies...");
        for (const auto& cu : m_context->compilationUnits) {
            updateDependencies(cu.second);
        }
        m_context->listener->onLog(LogSeverity::kInfo, "Success parsing dependencies");
    }

    void Live::update()
    {
        if (m_fileWatcher) {
            m_fileWatcher->update();
        }
        if (m_compiler) {
            m_compiler->update();
        }
    }

    void Live::setupFileWatcher()
    {
        std::vector<std::string> dirs;
        const auto& configDirs = m_context->liveConfig.directoriesToMonitor;
        if (!configDirs.empty()) {
            std::vector<std::string> existingDirs;
            for (const auto& el : configDirs) {
                auto p = TeenyPath::path{el};
                if (p.exists() && p.is_directory()) {
                    existingDirs.push_back(el);
                } else {
                    m_context->listener->onLog(
                        LogSeverity::kWarning, "Directory doesn't exist or is not a directory: " + el);
                }
            }

            if (existingDirs.empty()) {
                m_context->listener->onLog(LogSeverity::kError, "Delegate didn't provide any existing directories");
                return;
            }

            std::string directoriesStr;
            for (const auto& el : existingDirs) {
                directoriesStr.append("  ").append(el).append("\n");
            }
            directoriesStr.pop_back();  // last '\n' char
            m_context->listener->onLog(
                LogSeverity::kInfo, "Watching directories provided by delegate: \n" + directoriesStr);
            dirs = configDirs;
        } else {
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
            m_context->listener->onLog(
                LogSeverity::kInfo, "Watching directory substituted from compilation commands: \n  " + commonDir);
            dirs.push_back(commonDir);
        }

        m_context->dirsToMonitor = dirs;

        m_fileWatcher = jet::make_unique<FileWatcher>(dirs, [this](const FileWatcher::Event& event) {
            // Some IDEs doesn't 'modify' files, they 'move' it, so listening for both actions
            if (event.action != FileWatcher::Action::kModified && event.action != FileWatcher::Action::kMoved) {
                return;
            }

            auto fullPath = event.directory + event.filename;
            auto foundDeps = m_context->inverseDependencies.find(fullPath);
            if (foundDeps != m_context->inverseDependencies.end()) {
                for (const auto& filepath : foundDeps->second) {
                    auto foundCu = m_context->compilationUnits.find(filepath);
                    if (foundCu != m_context->compilationUnits.end()) {
                        const auto& cu = foundCu->second;
                        m_compiler->compile(
                            cu, [this, cu](int, const std::string&, const std::string&) { updateDependencies(cu); });
                    } else {
                        m_context->listener->onLog(LogSeverity::kError, "Cannot find dependency cu for " + filepath);
                    }
                }
            }
        });
    }

    void Live::tryReload()
    {
        m_context->listener->onLog(LogSeverity::kInfo, "Trying to reload code...");
        m_compiler->link([this](int status,
                             const std::string& libPath,
                             const std::vector<std::string>& objFilePaths,
                             const std::string&) {
            if (status != 0) {
                return;
            }

            m_context->listener->onCodePreLoad();

            m_context->listener->onLog(LogSeverity::kInfo, "Opening " + libPath + "...");
            auto libHandle = dlopen(libPath.c_str(), RTLD_NOW | RTLD_GLOBAL);  // NOLINT
            if (!libHandle) {
                m_context->listener->onLog(
                    LogSeverity::kError, "Cannot open library " + libPath + "\n" + std::string(dlerror()));
                m_context->listener->onCodePostLoad();
                return;
            }
            m_context->listener->onLog(LogSeverity::kInfo, "Library opened successfully");

            m_context->listener->onLog(LogSeverity::kInfo, "Loading symbols from " + libPath + "...");
            auto libSymbols = m_context->programInfoLoader->getProgramSymbols(m_context.get(), libPath);
            m_context->listener->onLog(LogSeverity::kInfo, "Symbols loaded");

            Program libProgram;
            libProgram.path = libPath;
            libProgram.symbols = libSymbols;
            libProgram.objFilePaths = objFilePaths;

            m_context->codeReloadPipeline->reload(m_context.get(), &libProgram);

            m_context->programs.emplace_back(std::move(libProgram));
            m_context->listener->onCodePostLoad();
        });
    }

    void Live::updateDependencies(const CompilationUnit& cu)
    {
        for (const auto& oldDep : m_context->dependencies[cu.sourceFilePath]) {
            m_context->inverseDependencies[oldDep].erase(cu.sourceFilePath);
        }
        auto cuDeps = m_context->dependenciesHandler->getDependencies(m_context.get(), cu);
        for (const auto& el : cuDeps) {
            m_context->inverseDependencies[el].insert(cu.sourceFilePath);
        }
        m_context->dependencies[cu.sourceFilePath] = std::move(cuDeps);
    }

    void Live::printInfo() { m_context->listener->onLog(LogSeverity::kDebug, toString(m_context->linkerType)); }
}
