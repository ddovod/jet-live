
#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "jet/live/ICompilationUnitsParser.hpp"
#include "jet/live/IDependenciesHandler.hpp"
#include "jet/live/LiveDelegate.hpp"

namespace jet
{
    struct LiveContext
    {
        std::unique_ptr<LiveDelegate> delegate;
        std::unique_ptr<ICompilationUnitsParser> compilationUnitsParser;
        std::unique_ptr<IDependenciesHandler> dependenciesHandler;
        std::vector<Program> programs;
        std::string thisExecutablePath;
        std::vector<std::string> dirsToMonitor;
        // sourceFilePath -> CompilationUnit
        std::unordered_map<std::string, CompilationUnit> compilationUnits;
        // sourceFilePath -> set of file paths
        std::unordered_map<std::string, std::unordered_set<std::string>> dependencies;
        // file path -> set of sourceFilePaths
        std::unordered_map<std::string, std::unordered_set<std::string>> inverseDependencies;
    };
}
