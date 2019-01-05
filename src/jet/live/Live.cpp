
#include "Live.hpp"
#include <cstring>
#include <dlfcn.h>
#include <subhook.h>
#include <teenypath.h>
#include "jet/live/Utility.hpp"

namespace jet
{
    Live::Live(std::unique_ptr<LiveDelegate>&& delegate)
        : m_context(jet::make_unique<LiveContext>())
    {
        if (delegate) {
            m_context->delegate = std::move(delegate);
        } else {
            m_context->delegate = jet::make_unique<LiveDelegate>();
        }
        m_context->thisExecutablePath = getExecutablePath();
        m_context->compilationUnitsParser = m_context->delegate->createCompilationUnitsParser();
        m_context->dependenciesHandler = m_context->delegate->createDependenciesHandler();
        m_context->programInfoLoader = m_context->delegate->createProgramInfoLoader();

        for (const auto& el : m_context->programInfoLoader->getAllLoadedProgramsPaths(m_context.get())) {
            if (el.empty()) {
                m_context->delegate->onLog(LogSeverity::kInfo, "Loading symbols of this process...");
            } else {
                m_context->delegate->onLog(LogSeverity::kInfo, "Loading symbols of " + el + "...");
            }
            Program program;
            program.path = el;
            program.symbols = m_context->programInfoLoader->getProgramSymbols(m_context.get(), program.path);
            m_context->delegate->onLog(LogSeverity::kInfo,
                "Symbols loaded successfully, total symbols: funcs " + std::to_string(program.symbols.functions.size())
                    + ", vars " + std::to_string(program.symbols.variables.size()));
            m_context->programs.push_back(std::move(program));
        }

        m_context->delegate->onLog(LogSeverity::kInfo, "Parsing compilation commands...");
        m_context->compilationUnits = m_context->compilationUnitsParser->parseCompilationUnits(m_context.get());
        if (m_context->compilationUnits.empty()) {
            m_context->delegate->onLog(LogSeverity::kError, "There're no compilation units");
            return;
        }

        m_context->delegate->onLog(LogSeverity::kInfo,
            "Success parsing compilation commands, total " + std::to_string(m_context->compilationUnits.size())
                + " compilation units");

        setupFileWatcher();

        m_compiler = jet::make_unique<Compiler>(m_context.get());

        m_context->delegate->onLog(LogSeverity::kInfo, "Parsing dependencies...");
        for (const auto& cu : m_context->compilationUnits) {
            updateDependencies(cu.second);
        }
        m_context->delegate->onLog(LogSeverity::kInfo, "Success parsing dependencies");
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
        auto delegateDirs = m_context->delegate->getDirectoriesToMonitor();
        if (!delegateDirs.empty()) {
            std::vector<std::string> existingDirs;
            for (const auto& el : delegateDirs) {
                auto p = TeenyPath::path{el};
                if (p.exists() && p.is_directory()) {
                    existingDirs.push_back(el);
                } else {
                    m_context->delegate->onLog(
                        LogSeverity::kWarning, "Directory doesn't exist or is not a directory: " + el);
                }
            }

            if (existingDirs.empty()) {
                m_context->delegate->onLog(LogSeverity::kError, "Delegate didn't provide any existing directories");
                return;
            }

            std::string directoriesStr;
            for (const auto& el : existingDirs) {
                directoriesStr.append("  ").append(el).append("\n");
            }
            directoriesStr.pop_back();  // last '\n' char
            m_context->delegate->onLog(
                LogSeverity::kInfo, "Watching directories provided by delegate: \n" + directoriesStr);
            dirs = delegateDirs;
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
            m_context->delegate->onLog(
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
                        m_context->delegate->onLog(LogSeverity::kError, "Cannot find dependency cu for " + filepath);
                    }
                }
            }
        });
    }

    void Live::tryReload()
    {
        m_context->delegate->onLog(LogSeverity::kInfo, "Trying to reload code...");
        m_compiler->link([this](int status, const std::string& libPath, const std::string&) {
            if (status != 0) {
                return;
            }

            m_context->delegate->onCodePreLoad();

            m_context->delegate->onLog(LogSeverity::kInfo, "Opening " + libPath + "...");
            auto libHandle = dlopen(libPath.c_str(), RTLD_NOW | RTLD_GLOBAL);  // NOLINT
            if (!libHandle) {
                m_context->delegate->onLog(
                    LogSeverity::kError, "Cannot open library " + libPath + "\n" + std::string(dlerror()));
                m_context->delegate->onCodePostLoad();
                return;
            }
            m_context->delegate->onLog(LogSeverity::kInfo, "Library opene successfully");

            m_context->delegate->onLog(LogSeverity::kInfo, "Loading symbols from " + libPath + "...");
            auto libSymbols = m_context->programInfoLoader->getProgramSymbols(m_context.get(), libPath);
            m_context->delegate->onLog(LogSeverity::kInfo,
                "Symbols loaded successfully, total symbols: funcs " + std::to_string(libSymbols.functions.size())
                    + ", vars " + std::to_string(libSymbols.variables.size()));

            m_context->delegate->onLog(LogSeverity::kInfo, "Reloading old code with new one...");
            int functionsReloaded = 0;
            for (const auto& sym : libSymbols.functions) {
                void* oldFuncPtr = nullptr;
                for (auto it = m_context->programs.rbegin(); it != m_context->programs.rend(); it++) {
                    auto foundSym = it->symbols.functions.find(sym.second.name);
                    if (foundSym != it->symbols.functions.end()) {
                        oldFuncPtr = reinterpret_cast<void*>(foundSym->second.runtimeAddress);
                        break;
                    }
                }

                if (!oldFuncPtr) {
                    continue;
                }

                auto newFuncPtr = reinterpret_cast<void*>(sym.second.runtimeAddress);
                if (!newFuncPtr) {
                    continue;
                }

                auto hook = subhook_new(oldFuncPtr, newFuncPtr, SUBHOOK_64BIT_OFFSET);
                if (auto subhookStatus = subhook_install(hook)) {
                    m_context->delegate->onLog(LogSeverity::kError,
                        "Cannot hook function: " + sym.second.name + ", status " + std::to_string(subhookStatus));
                } else {
                    functionsReloaded++;
                }
            }

            int variablesTransferred = 0;
            for (const auto& sym : libSymbols.variables) {
                void* oldVarPtr = nullptr;
                size_t oldVarSize = 0;
                for (auto it = m_context->programs.rbegin(); it != m_context->programs.rend(); it++) {
                    auto foundSym = it->symbols.variables.find(sym.second.name);
                    if (foundSym != it->symbols.variables.end()) {
                        oldVarSize = foundSym->second.size;
                        oldVarPtr = reinterpret_cast<void*>(foundSym->second.runtimeAddress);
                        break;
                    }
                }

                if (!oldVarPtr) {
                    continue;
                }

                auto newVarPtr = reinterpret_cast<void*>(sym.second.runtimeAddress);
                if (!newVarPtr) {
                    continue;
                }

                // Trying to do our best
                memcpy(newVarPtr, oldVarPtr, std::min(sym.second.size, oldVarSize));
                variablesTransferred++;
            }

            Program libProgram;
            libProgram.path = libPath;
            libProgram.symbols = libSymbols;
            m_context->programs.push_back(std::move(libProgram));

            m_context->delegate->onCodePostLoad();

            m_context->delegate->onLog(LogSeverity::kInfo,
                "Code reloaded successfully: funcs " + std::to_string(functionsReloaded) + "/"
                    + std::to_string(libSymbols.functions.size()) + ", vars " + std::to_string(variablesTransferred)
                    + "/" + std::to_string(libSymbols.variables.size()));
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
}
