
#include "LinkTimeRelocationsStep.hpp"
#include <limits>
#include "jet/live/DataTypes.hpp"
#include "jet/live/LiveContext.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    void LinkTimeRelocationsStep::reload(LiveContext* context, Program* newProgram)
    {
        context->listener->onLog(LogSeverity::kInfo, "Loading link-time relocations...");

        const auto& relocs = context->programInfoLoader->getStaticRelocations(context, newProgram->objFilePaths);
        auto totalRelocs = relocs.size();
        size_t appliedRelocs = 0;
        for (const auto& reloc : relocs) {
            const Symbol* targetSymbol =
                findFunction(newProgram->symbols, reloc.targetSymbolName, reloc.targetSymbolHash);
            if (!targetSymbol) {
                context->listener->onLog(LogSeverity::kError, "targetSymbol not fount: " + reloc.targetSymbolName);
                continue;
            }

            const Symbol* relocSymbol =
                findVariable(newProgram->symbols, reloc.relocationSymbolName, reloc.relocationSymbolHash);
            if (!relocSymbol) {
                context->listener->onLog(LogSeverity::kError, "relocSymbol not found: " + reloc.relocationSymbolName);
                continue;
            }

            const Symbol* oldVar = nullptr;
            auto& progs = context->programs;
            for (size_t i = progs.size(); i > 0 && !oldVar; i--) {
                auto& prog = progs[i - 1];
                oldVar = findVariable(prog.symbols, reloc.relocationSymbolName, reloc.relocationSymbolHash);
            }
            if (!oldVar) {
                continue;
            }

            auto relocAddressVal = targetSymbol->runtimeAddress + reloc.relocationOffsetRelativeTargetSymbolAddress;
            int32_t* relocAddress = reinterpret_cast<int32_t*>(relocAddressVal);
            if (!unprotect(relocAddress, 4)) {
                context->listener->onLog(LogSeverity::kError, "'unprotect' failed");
                continue;
            }

            auto distance = std::abs(static_cast<intptr_t>(oldVar->runtimeAddress - relocSymbol->runtimeAddress));
            if (distance > std::numeric_limits<int32_t>::max()) {
                context->listener->onLog(
                    LogSeverity::kWarning, "Cannot relocate variable, distance doesn't fit into 'int32'");
                continue;
            }

            *relocAddress += oldVar->runtimeAddress - relocSymbol->runtimeAddress;
            context->listener->onLog(LogSeverity::kInfo, relocSymbol->name + " was relocated");

            auto& newVars = newProgram->symbols.variables[relocSymbol->name];
            for (size_t i = 0; i < newVars.size(); i++) {
                if (newVars[i].hash == relocSymbol->hash) {
                    newVars.erase(newVars.begin() + i);
                    break;
                }
            }
            if (newVars.empty()) {
                newProgram->symbols.variables.erase(relocSymbol->name);
            }
            appliedRelocs++;
        }

        context->listener->onLog(LogSeverity::kInfo,
            "Done, relocated: " + std::to_string(appliedRelocs) + "/" + std::to_string(totalRelocs));
    }
}
