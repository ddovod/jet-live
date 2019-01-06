
#include "LiveDelegate.hpp"
#include "jet/live/CompileCommandsCompilationUnitsParser.hpp"
#include "jet/live/DefaultProgramInfoLoader.hpp"
#include "jet/live/DepfileDependenciesHandler.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    void LiveDelegate::onLog(LogSeverity, const std::string&) {}

    void LiveDelegate::onCodePreLoad() {}

    void LiveDelegate::onCodePostLoad() {}

    size_t LiveDelegate::getWorkerThreadsCount() { return 4; }

    std::vector<std::string> LiveDelegate::getDirectoriesToMonitor() { return {}; }

    bool LiveDelegate::shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol)
    {
        return (symbol.external && symbol.type == MachoSymbolType::kSection
                && symbol.sectionIndex == context.textSectionIndex && !symbol.weakDef);
    }

    bool LiveDelegate::shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol)
    {
        static const std::string textSectionName = ".text";
        return (symbol.type == ElfSymbolType::kFunction && symbol.size != 0
                && symbol.sectionIndex < context.sectionNames.size()  // Some sections has reserved indices
                && context.sectionNames[symbol.sectionIndex] == textSectionName);
    }

    bool LiveDelegate::shouldTransferMachoSymbol(const MachoContext&, const MachoSymbol&) { return false; }

    bool LiveDelegate::shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol)
    {
        static const std::string bssSectionName = ".bss";
        return (symbol.type == ElfSymbolType::kObject && symbol.sectionIndex < context.sectionNames.size()
                && context.sectionNames[symbol.sectionIndex] == bssSectionName);
    }

    std::unique_ptr<ICompilationUnitsParser> LiveDelegate::createCompilationUnitsParser()
    {
        return jet::make_unique<CompileCommandsCompilationUnitsParser>();
    }

    std::unique_ptr<IDependenciesHandler> LiveDelegate::createDependenciesHandler()
    {
        return jet::make_unique<DepfileDependenciesHandler>();
    }

    std::unique_ptr<IProgramInfoLoader> LiveDelegate::createProgramInfoLoader()
    {
        return jet::make_unique<DefaultProgramInfoLoader>();
    }
}
