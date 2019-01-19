
#pragma once

#include "jet/live/IProgramInfoLoader.hpp"

namespace jet
{
    /**
     * Program info loader implementation for mach-o.
     */
    class MachoProgramInfoLoader : public IProgramInfoLoader
    {
    public:
        std::vector<std::string> getAllLoadedProgramsPaths(const LiveContext* context) const override;
        Symbols getProgramSymbols(const LiveContext* context, const std::string& filepath) const override;
        std::vector<Relocation> getLinkTimeRelocations(const LiveContext* context,
            const std::vector<std::string>& objFilePaths) override;
    };
}
