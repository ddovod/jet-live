
#pragma once

namespace jet
{
    struct MachoContext;
    struct MachoSymbol;
    struct ElfContext;
    struct ElfSymbol;

    /**
     * Base class for symbols filtering strategy.
     */
    class ISymbolsFilter
    {
    public:
        virtual ~ISymbolsFilter() {}

        /**
         * Checks if this mach-o symbol should be treated as reloadable function.
         * You shoul have some idea about what is mach-o to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldReloadMachoSymbol(const MachoContext& context, const MachoSymbol& symbol) = 0;

        /**
         * Checks if this elf symbol should be treated as reloadable function.
         * You shoul have some idea about what is elf to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldReloadElfSymbol(const ElfContext& context, const ElfSymbol& symbol) = 0;

        /**
         * Checks if this mach-o symbol should be treated as transferrable static variable.
         * You shoul have some idea about what is mach-o to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldTransferMachoSymbol(const MachoContext& context, const MachoSymbol& symbol) = 0;

        /**
         * Checks if this elf symbol should be treated as transferrable static variable.
         * You shoul have some idea about what is elf to override use this function.
         * Please see implementation for default behaviour.
         */
        virtual bool shouldTransferElfSymbol(const ElfContext& context, const ElfSymbol& symbol) = 0;
    };
}
