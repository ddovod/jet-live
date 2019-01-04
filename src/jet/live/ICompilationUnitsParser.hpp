
#pragma once

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
    };
}
