
#include "DepfileDependenciesHandler.hpp"
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <teenypath.h>
#include "jet/live/LiveContext.hpp"

namespace jet
{
    std::unordered_set<std::string> DepfileDependenciesHandler::getDependencies(const LiveContext* context,
        CompilationUnit& cu)
    {
        std::unordered_set<std::string> deps;
        deps.insert(cu.sourceFilePath);

        // Trying deal with "filename.cpp.o.d" vs "filename.cpp.d" depfile names
        if (cu.depFilePath.empty()) {
            TeenyPath::path depfilePath{std::string(cu.objFilePath).append(".d")};
            if (depfilePath.exists()) {
                cu.depFilePath = depfilePath.string();
            }
        }

        if (cu.depFilePath.empty()) {
            auto depfilePath = cu.objFilePath;
            depfilePath.back() = 'd';
            if (TeenyPath::path{depfilePath}.exists()) {
                cu.depFilePath = depfilePath;
            }
        }

        if (cu.depFilePath.empty()) {
            context->events->addLog(LogSeverity::kWarning, "Empty depfile path for cu: " + cu.sourceFilePath);
            return deps;
        }

        std::ifstream f{cu.depFilePath};
        if (!f.is_open()) {
            context->events->addLog(LogSeverity::kWarning, "Cannot open depfile: " + cu.depFilePath);
            return deps;
        }

        std::string line;
        // First line is a path to the .o file
        std::getline(f, line);
        while (std::getline(f, line)) {
            line.erase(0, line.find_first_not_of(' '));
            bool matches = false;
            for (const auto& dir : context->dirFilters) {
                if (line.find(dir) != std::string::npos) {
                    matches = true;
                    break;
                }
            }
            if (!matches) {
                continue;
            }

            line.erase(line.find_last_not_of(" \\") + 1);

            auto found = line.find(' ');
            if (found == std::string::npos) {
                deps.insert(TeenyPath::path{line}.resolve_absolute().string());
            } else {
                std::size_t prev = 0;
                while (found != std::string::npos) {
                    deps.insert(TeenyPath::path{line.substr(prev, found - prev)}.resolve_absolute().string());
                    prev = found + 1;
                    found = line.find(' ', prev);
                }
                deps.insert(TeenyPath::path{line.substr(prev, found - prev)}.resolve_absolute().string());
            }
        }

        return deps;
    }
}
