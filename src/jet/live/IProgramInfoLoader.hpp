
#pragma once

#include <string>
#include <vector>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    struct LiveContext;
    class IProgramInfoLoader
    {
    public:
        virtual ~IProgramInfoLoader() {}

        /**
         * Retrieves executable and shared libraries paths loaded into this process memory.
         */
        virtual std::vector<std::string> getAllLoadedProgramsPaths(const LiveContext* context) const = 0;

        /**
         * Retrieves symbols of given program.
         * \param context LiveContext pointer.
         * \param filepath Path to the program file. Empty string for this executable.
         */
        virtual Symbols getProgramSymbols(const LiveContext* context, const std::string& filepath) const = 0;

        /**
         * Retrieves a link-time relocations in the `text`
         * section which use symbols from `bss` and `data` sections.
         * Used to fix static/global variable addresses in new code to
         * make them pointing to corresponding variables in old code.
         */
        virtual std::vector<Relocation> getLinkTimeRelocations(const LiveContext* context,
            const std::vector<std::string>& objFilePaths) = 0;
    };
}
