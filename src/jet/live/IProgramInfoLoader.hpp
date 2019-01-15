
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

        // TODO: docs
        virtual std::vector<Relocation> getStaticRelocations(const LiveContext* context,
            const std::vector<std::string>& objFilePaths) = 0;
    };
}
