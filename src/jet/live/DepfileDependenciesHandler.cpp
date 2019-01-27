
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
            context->listener->onLog(LogSeverity::kWarning, "Empty depfile path for cu: " + cu.sourceFilePath);
            return deps;
        }

        std::ifstream f{cu.depFilePath};
        if (!f.is_open()) {
            context->listener->onLog(LogSeverity::kWarning, "Cannot open depfile: " + cu.depFilePath);
            return deps;
        }

        std::string line;
        // First line is a path to the .o file
        std::getline(f, line);
        while (std::getline(f, line)) {
            // Moving 2 whitespaces to the end
            std::rotate(line.begin(), line.begin() + 2, line.end());
            bool skip = false;
            for (const auto& dir : context->dirsToMonitor) {
                if (line.find(dir) == std::string::npos) {
                    skip = true;
                    break;
                }
            }
            if (skip) {
                continue;
            }

            line.pop_back();
            line.pop_back();
            if (line.back() == '\\') {
                line.pop_back();
                line.pop_back();
            }

            deps.insert(TeenyPath::path{line}.resolve_absolute().string());
        }

        return deps;
    }
}
