
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    class ICompilationUnitsParser;
    class IDependenciesHandler;
    class IProgramInfoLoader;

    enum class LogSeverity
    {
        kDebug,
        kInfo,
        kWarning,
        kError
    };

    /**
     * Base class for custom delegate.
     */
    class LiveDelegate
    {
    public:
        virtual ~LiveDelegate() {}

        /**
         * Called on each log message from the library.
         */
        virtual void onLog(LogSeverity severity, const std::string& message);

        /**
         * Called right before shared library with new code is loaded into the process address space.
         */
        virtual void onCodePreLoad();

        /**
         * Called right after all functions are hooked and state is transferred.
         */
        virtual void onCodePostLoad();

        /**
         * The maximum amount of possible worker threads used by the library.
         * Usually all these threads are busy compiling new code.
         * 4 by default.
         */
        virtual size_t getWorkerThreadsCount();

        /**
         * A list of directories to monitor for source file changes.
         * Empty by default, so the most common directory for all found compilation units will be used.
         */
        virtual std::vector<std::string> getDirectoriesToMonitor();

        /**
         * Checks if this mach-o symbol should be treated as reloadable function.
         * You shoul have some idea about what is mach-o to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol);

        /**
         * Checks if this elf symbol should be treated as reloadable function.
         * You shoul have some idea about what is elf to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol);

        /**
         * Checks if this mach-o symbol should be treated as transferrable static variable.
         * You shoul have some idea about what is mach-o to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldTransferMachoSymbol(const MachoContext& context, const MachoSymbol& symbol);

        /**
         * Checks if this elf symbol should be treated as transferrable static variable.
         * You shoul have some idea about what is elf to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol);

        /**
         * Creates a compilation units parser instance.
         * By default it is \ref jet::CompileCommandsCompilationUnitsParser .
         */
        virtual std::unique_ptr<ICompilationUnitsParser> createCompilationUnitsParser();

        /**
         * Creates a dependency handler instance.
         * By default it is \ref jet::DepfileDependencyHandler .
         */
        virtual std::unique_ptr<IDependenciesHandler> createDependenciesHandler();

        /**
         * Created a program info loader instance.
         * By default it is \ref jet::ElfProgramInfoLoader for linux and
         * \ref jet::MachoProgramInfoLoader for macos.
         */
        virtual std::unique_ptr<IProgramInfoLoader> createProgramInfoLoader();
    };
}
