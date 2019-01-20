
#pragma once

#include <teenypath.h>
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
        void updateCompilationUnits(LiveContext* context,
            const std::string& filepath,
            std::vector<std::string>* addedCompilationUnits,
            std::vector<std::string>* modifiedCompilationUnits,
            std::vector<std::string>* removedCompilationUnits) override;

    protected:
        TeenyPath::path m_compileCommandsPath;

        std::unordered_map<std::string, CompilationUnit> parseCompilationUnitsInternal(const LiveContext* context,
            const TeenyPath::path& filepath);

        /**
         * By default it tries to find `compile_commands.json` in the executable directory
         * and all parent directories recursively.
         * For custom `compile_commands.json` location you can subclass and override this method.
         */
        virtual TeenyPath::path getCompileCommandsPath(const LiveContext* context) const;
    };
}
