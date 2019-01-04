
#pragma once

#include "jet/live/ICompilationUnitsParser.hpp"

namespace jet
{
    /**
     * Compilation units parser based on `compile_commands.json` file.
     */
    class CompileCommandsCompilationUnitsParser : public ICompilationUnitsParser
    {
    public:
        std::unordered_map<std::string, CompilationUnit> parseCompilationUnits(const LiveContext* context) override;

    protected:
        /**
         * By default it tries to find `compile_commands.json` in the executable directory
         * and all parent directories recursively.
         * For custom `compile_commands.json` location you can subclass and override this method.
         */
        virtual std::string getCompileCommandsPath(const LiveContext* context) const;
    };
}
