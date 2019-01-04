
#pragma once

#include "jet/live/ICompilationUnitsParser.hpp"

namespace jet
{
    class CompileCommandsCompilationUnitsParser : public ICompilationUnitsParser
    {
    public:
        std::unordered_map<std::string, CompilationUnit> parseCompilationUnits(const LiveContext* context) override;

    protected:
        std::string getCompileCommandsPath(const LiveContext* context) const;
    };
}
