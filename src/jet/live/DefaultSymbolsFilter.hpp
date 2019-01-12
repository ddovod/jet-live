
#pragma once

#include "jet/live/ISymbolsFilter.hpp"

namespace jet
{
    /**
     * Default implementation of symbols filter.
     */
    class DefaultSymbolsFilter : public ISymbolsFilter
    {
    public:
        bool shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol) override;
        bool shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol) override;
        bool shouldTransferMachoSymbol(const MachoContext& context, const MachoSymbol& symbol) override;
        bool shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol) override;
    };
}
