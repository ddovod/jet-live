
#pragma once

#include "jet/live/IDependenciesHandler.hpp"

namespace jet
{
    /**
     * Dependencies handler implementation based on compiler generated depfiles.
     * For more info please refer to `-MD` compiler option.
     */
    class DepfileDependenciesHandler : public IDependenciesHandler
    {
    public:
        std::unordered_set<std::string> getDependencies(const LiveContext* context, const CompilationUnit& cu) override;
    };
}
