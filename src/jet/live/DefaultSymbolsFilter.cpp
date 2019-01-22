
#include "DefaultSymbolsFilter.hpp"
#include "jet/live/DataTypes.hpp"

namespace
{
    const std::string& getStringOr(const std::vector<std::string>& vec, size_t index, const std::string& fallback)
    {
        return vec.size() <= index ? fallback : vec[index];
    }
}

namespace jet
{
    bool DefaultSymbolsFilter::shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol)
    {
        static const std::string textSectionName = "__text";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == MachoSymbolType::kSection && !symbol.weakDef && sectionName == textSectionName);
    }

    bool DefaultSymbolsFilter::shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol)
    {
        static const std::string textSectionName = ".text";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == ElfSymbolType::kFunction && symbol.size != 0 && sectionName == textSectionName);
    }

    bool DefaultSymbolsFilter::shouldTransferMachoSymbol(const MachoContext& context, const MachoSymbol& symbol)
    {
        static const std::string bssSectionName = "__bss";
        static const std::string dataSectionName = "__data";
        static const std::string commonSectionName = "__common";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (
            symbol.type == MachoSymbolType::kSection && !symbol.privateExternal && !symbol.weakDef
            && (sectionName == bssSectionName || sectionName == dataSectionName || sectionName == commonSectionName));
    }

    bool DefaultSymbolsFilter::shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol)
    {
        static const std::string bssSectionName = ".bss";
        static const std::string dataSectionName = ".data";
        const auto& sectionName = getStringOr(context.sectionNames, symbol.sectionIndex, "?");
        return (symbol.type == ElfSymbolType::kObject && symbol.binding == ElfSymbolBinding::kLocal
                && symbol.visibility == ElfSymbolVisibility::kDefault
                && (sectionName == bssSectionName || sectionName == dataSectionName));
    }
}
