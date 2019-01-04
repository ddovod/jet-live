
#pragma once

#include "jet/live/IDependenciesHandler.hpp"

namespace jet
{
    class DepfileDependenciesHandler : public IDependenciesHandler
    {
    public:
        std::unordered_set<std::string> getDependencies(const LiveContext* context, const CompilationUnit& cu) override;
    };
}
