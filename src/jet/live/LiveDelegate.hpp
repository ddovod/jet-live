
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    class ICompilationUnitsParser;
    class IDependenciesHandler;

    enum class LogSeverity
    {
        kDebug,
        kInfo,
        kWarning,
        kError
    };

    class LiveDelegate
    {
    public:
        virtual ~LiveDelegate() {}

        virtual void onLog(LogSeverity severity, const std::string& message);
        virtual void onCodePreLoad();
        virtual void onCodePostLoad();

        virtual size_t getWorkerThreadsCount();
        virtual std::vector<std::string> getDirectoriesToMonitor();
        virtual bool shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol);
        virtual bool shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol);
        virtual bool shouldTransferMachoSymbol(const MachoContext& context, const MachoSymbol& symbol);
        virtual bool shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol);

        virtual std::unique_ptr<ICompilationUnitsParser> createCompilationUnitsParser();
        virtual std::unique_ptr<IDependenciesHandler> createDependenciesHandler();
    };
}
