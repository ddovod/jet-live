
#include "CompileCommandsCompilationUnitsParser.hpp"
#include <argh.h>
#include <fstream>
#include <json.hpp>
#include <teenypath.h>
#include <wordexp.h>
#include "jet/live/DataTypes.hpp"
#include "jet/live/LiveContext.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    std::unordered_map<std::string, CompilationUnit> CompileCommandsCompilationUnitsParser::parseCompilationUnits(
        const LiveContext* context)
    {
        auto probablyDbPath = getCompileCommandsPath(context);
        if (!probablyDbPath.exists()) {
            context->events->addLog(
                LogSeverity::kError, "Cannot find 'compile_commands.json' path at " + probablyDbPath.string());
            return {};
        }

        return parseCompilationUnitsInternal(context, probablyDbPath);
    }

    bool CompileCommandsCompilationUnitsParser::updateCompilationUnits(LiveContext* context,
        const std::string& filepath,
        std::vector<std::string>* addedCompilationUnits,
        std::vector<std::string>* modifiedCompilationUnits,
        std::vector<std::string>* removedCompilationUnits)
    {
        TeenyPath::path path{filepath};
        if (!path.exists() || !(m_compileCommandsPath == path.resolve_absolute())) {
            return false;
        }

        context->events->addLog(LogSeverity::kInfo, "Updating compilation units...");

        auto oldCompilationUnits = context->compilationUnits;
        auto newCompilationUnits = parseCompilationUnitsInternal(context, m_compileCommandsPath);
        for (const auto& oldCu : oldCompilationUnits) {
            auto found = newCompilationUnits.find(oldCu.first);
            if (found == newCompilationUnits.end()) {
                removedCompilationUnits->push_back(oldCu.first);
            } else {
                if (oldCu.second.compilationCommandStr != found->second.compilationCommandStr) {
                    modifiedCompilationUnits->push_back(oldCu.first);
                }
            }
        }

        for (const auto& newCu : newCompilationUnits) {
            if (oldCompilationUnits.find(newCu.first) == oldCompilationUnits.end()) {
                addedCompilationUnits->push_back(newCu.first);
            }
        }

        context->compilationUnits = newCompilationUnits;

        context->events->addLog(LogSeverity::kInfo,
            "ADDED: " + std::to_string(addedCompilationUnits->size())
                + ", MODIFIED: " + std::to_string(modifiedCompilationUnits->size())
                + ", REMOVED: " + std::to_string(removedCompilationUnits->size()));

        return !addedCompilationUnits->empty() || !modifiedCompilationUnits->empty()
               || !removedCompilationUnits->empty();
    }

    std::unordered_map<std::string, CompilationUnit>
        CompileCommandsCompilationUnitsParser::parseCompilationUnitsInternal(const LiveContext* context,
            const TeenyPath::path& filepath)
    {
        using nlohmann::json;

        std::unordered_map<std::string, CompilationUnit> res;

        // Parsing `compile_commands.json`
        auto probablyDbPath = filepath;
        context->events->addLog(LogSeverity::kDebug, "Reading `compile_commands.json` from " + probablyDbPath.string());
        std::ifstream f{probablyDbPath.string()};
        if (!f.is_open()) {
            context->events->addLog(
                LogSeverity::kError, "Cannot open 'compile_commands.json' at " + probablyDbPath.string());
            return res;
        }

        m_compileCommandsPath = probablyDbPath;

        json dbJson;
        f >> dbJson;
        for (const auto& cmdJson : dbJson) {
            CompilationUnit cu;
            cu.compilationCommandStr = cmdJson["command"];
            cu.compilationDirStr = cmdJson["directory"];
            cu.compilationDirStr = TeenyPath::path(cu.compilationDirStr).resolve_absolute().string();
            cu.sourceFilePath = cmdJson["file"];
            TeenyPath::path sourceFilePath{cu.sourceFilePath};
            if (!sourceFilePath.is_absolute()) {
                sourceFilePath = TeenyPath::path{cu.compilationDirStr} / sourceFilePath;
                cu.sourceFilePath = sourceFilePath.resolve_absolute().string();
            } else {
                cu.sourceFilePath = TeenyPath::path(cu.sourceFilePath).resolve_absolute().string();
            }

            wordexp_t result;
            switch (wordexp(cu.compilationCommandStr.c_str(), &result, 0)) {
                case 0: break;
                case WRDE_NOSPACE: wordfree(&result); continue;
                default: continue;
            }

            argh::parser parser(
                static_cast<int>(result.we_wordc), result.we_wordv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
            cu.objFilePath = parser({"-o", "--output"}).str();
            if (cu.objFilePath.empty()) {
                context->events->addLog(
                    LogSeverity::kWarning, "Cannot find object file path, skipping: " + cu.sourceFilePath);
                continue;
            }
            cu.hasColorDiagnosticsFlag = parser[{"-fcolor-diagnostics"}];
            cu.depFilePath = parser({"-MF"}).str();

            TeenyPath::path objFilePath{cu.objFilePath};
            if (objFilePath.is_absolute()) {
                cu.objFilePath = objFilePath.string();
            } else {
                cu.objFilePath = (TeenyPath::path{cu.compilationDirStr} / objFilePath).string();
            }

            cu.compilerPath = parser[0];
            res[cu.sourceFilePath] = cu;
        }

        return res;
    }

    TeenyPath::path CompileCommandsCompilationUnitsParser::getCompileCommandsPath(const LiveContext* context) const
    {
        // Trying to find `compile_commands.json` in current and parent directories
        auto dbPath = TeenyPath::path{context->thisExecutablePath}.parent_path() / "compile_commands.json";
        while (!dbPath.exists() && !dbPath.is_empty()) {
            dbPath = dbPath.parent_path().parent_path() / "compile_commands.json";
        }
        return dbPath.resolve_absolute();
    }
}
