
#include "LiveDelegate.hpp"
#include "jet/live/CompileCommandsCompilationUnitsParser.hpp"
#include "jet/live/DefaultProgramInfoLoader.hpp"
#include "jet/live/DepfileDependenciesHandler.hpp"
#include "jet/live/Utility.hpp"

namespace
{
    const std::string& getStringOr(const std::vector<std::string>& vec, size_t index, const std::string& fallback)
    {
        return vec.size() <= index ? fallback : vec[index];
    }
}

namespace jet
{
    void LiveDelegate::onLog(LogSeverity, const std::string&) {}

    void LiveDelegate::onCodePreLoad() {}

    void LiveDelegate::onCodePostLoad() {}

    size_t LiveDelegate::getWorkerThreadsCount() { return 4; }

    std::vector<std::string> LiveDelegate::getDirectoriesToMonitor() { return {}; }

    bool LiveDelegate::shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol)
    {
        static const std::string textSectionName = "__text";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == MachoSymbolType::kSection && symbol.weakDef == false && sectionName == textSectionName);
    }

    bool LiveDelegate::shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol)
    {
        static const std::string textSectionName = ".text";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == ElfSymbolType::kFunction && symbol.size != 0 && sectionName == textSectionName);
    }

    bool LiveDelegate::shouldTransferMachoSymbol(const MachoContext& context, const MachoSymbol& symbol)
    {
        // TODO(ddovod): think about '__data', '__const' and '__common' sections
        static const std::string bssSectionName = "__bss";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == MachoSymbolType::kSection && sectionName == bssSectionName);
    }

    bool LiveDelegate::shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol)
    {
        // TODO(ddovod): think about '.data' section
        static const std::string bssSectionName = ".bss";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == ElfSymbolType::kObject && sectionName == bssSectionName);
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
