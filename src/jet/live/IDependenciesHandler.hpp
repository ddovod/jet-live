
#pragma once

#include <unordered_set>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    struct LiveContext;
    class IDependenciesHandler
    {
    public:
        virtual ~IDependenciesHandler() {}

        virtual std::unordered_set<std::string> getDependencies(const LiveContext* context,
            const CompilationUnit& cu) = 0;
    };
}
