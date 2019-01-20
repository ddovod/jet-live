
#pragma once

#include <string>
#include <vector>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    struct LiveContext;
    /**
     * Compilation units parser interface.
     */
    class ICompilationUnitsParser
    {
    public:
        virtual ~ICompilationUnitsParser() {}

        /**
         * Parses and returns info about all compilation units used to construct this application.
         * \return "cu source path" -> "compilation unit" map
         */
        virtual std::unordered_map<std::string, CompilationUnit> parseCompilationUnits(const LiveContext* context) = 0;

        /**
         * Updates compilation units list inside `context` using `filepath`, if this `filepath` is a source of
         * compilation units. Fills output vectors with added, modified and removed compilation units.
         */
        virtual void updateCompilationUnits(LiveContext* context,
            const std::string& filepath,
            std::vector<std::string>* addedCompilationUnits,
            std::vector<std::string>* modifiedCompilationUnits,
            std::vector<std::string>* removedCompilationUnits) = 0;
    };
}
