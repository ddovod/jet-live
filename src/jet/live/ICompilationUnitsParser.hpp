
#pragma once

#include "jet/live/DataTypes.hpp"

namespace jet
{
    struct LiveContext;
    class ICompilationUnitsParser
    {
    public:
        virtual ~ICompilationUnitsParser() {}

        virtual std::unordered_map<std::string, CompilationUnit> parseCompilationUnits(const LiveContext* context) = 0;
    };
}
