
#pragma once

#include <unordered_set>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    struct LiveContext;
    /**
     * Dependencies handler interface.
     */
    class IDependenciesHandler
    {
    public:
        virtual ~IDependenciesHandler() {}

        /**
         * Finds dependencies of given cu (files which cu depends on).
         * \return A set of dependencies file paths.
         */
        virtual std::unordered_set<std::string> getDependencies(const LiveContext* context,
            const CompilationUnit& cu) = 0;
    };
}
